#pragma once
#include "stdafx.h"
#include "D3D12Timer.h"

namespace D3D12
{
	D3D12Timer::D3D12Timer()
	{

	}

	// Destructor.
	D3D12Timer::~D3D12Timer()
	{
		if (m_pQueryHeap_)
			m_pQueryHeap_->Release();

		if (m_pQueryResourceCPU)
			m_pQueryResourceCPU->Release();

		if (m_pQueryResourceGPU)
			m_pQueryResourceGPU->Release();
	}

	HRESULT D3D12Timer::Init(ID3D12Device* pDevice, UINT numTimers)
	{
		HRESULT hr = S_OK;
		m_pDevice = pDevice;

		m_TimerCount = numTimers;

		D3D12_QUERY_HEAP_DESC queryHeapDesc;
		queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		queryHeapDesc.NodeMask = 0;
		queryHeapDesc.Count = m_TimerCount * 2;

		if (SUCCEEDED(hr = m_pDevice->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_pQueryHeap_))))
		{
			D3D12_RESOURCE_DESC resouceDesc;
			ZeroMemory(&resouceDesc, sizeof(resouceDesc));
			resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resouceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
			resouceDesc.Width = sizeof(GPUTimestampPair) * m_TimerCount;
			resouceDesc.Height = 1;
			resouceDesc.DepthOrArraySize = 1;
			resouceDesc.MipLevels = 1;
			resouceDesc.Format = DXGI_FORMAT_UNKNOWN;
			resouceDesc.SampleDesc.Count = 1;
			resouceDesc.SampleDesc.Quality = 0;
			resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resouceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12_HEAP_PROPERTIES heapProp = {};
			heapProp.Type = D3D12_HEAP_TYPE_READBACK;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProp.CreationNodeMask = 1;
			heapProp.VisibleNodeMask = 1;

			if (SUCCEEDED(hr = m_pDevice->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resouceDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&m_pQueryResourceCPU))
			))
			{
				m_pQueryResourceCPU->SetName(L"queryResourceCPU_");
			}



			//create gpu dest
			//resouceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
			if (SUCCEEDED(hr = m_pDevice->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resouceDesc,
				D3D12_RESOURCE_STATE_COPY_SOURCE,
				nullptr,
				IID_PPV_ARGS(&m_pQueryResourceGPU))
			))
			{
				m_pQueryResourceGPU->SetName(L"queryResourceGPU_");
			}
		}

		return hr;
	}

	// Start timestamp.
	void D3D12Timer::Start(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndex)
	{
		m_Active = true;

		pCommandList->EndQuery(m_pQueryHeap_, D3D12_QUERY_TYPE_TIMESTAMP, timestampPairIndex * 2);
	}

	// Stop timestamp.
	void D3D12Timer::Stop(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndex)
	{
		m_Active = false;

		pCommandList->EndQuery(m_pQueryHeap_, D3D12_QUERY_TYPE_TIMESTAMP, timestampPairIndex * 2 + 1);
	}

	// Resolve query data. Write query to m_pDevice memory. Make sure to wait for query to finsih before resolving data.
	void D3D12Timer::ResolveQueryToCPU(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndex)
	{
		pCommandList->ResolveQueryData(
			m_pQueryHeap_,
			D3D12_QUERY_TYPE_TIMESTAMP,
			timestampPairIndex * 2,
			2,
			m_pQueryResourceCPU,
			sizeof(GPUTimestampPair) * timestampPairIndex
		);
	}

	void D3D12Timer::ResolveQueryToCPU(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndexFirst, UINT timestampPairIndexLast)
	{
		UINT numToResolve = timestampPairIndexLast - timestampPairIndexFirst;
		pCommandList->ResolveQueryData(
			m_pQueryHeap_,
			D3D12_QUERY_TYPE_TIMESTAMP,
			timestampPairIndexFirst * 2,
			numToResolve * 2,
			m_pQueryResourceCPU,
			sizeof(GPUTimestampPair) * timestampPairIndexFirst
		);
	}

	void D3D12Timer::ResolveQueryToGPU(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource** ppQueryResourceGPUOut)
	{
		setGPUResourceState(pCommandList, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

		pCommandList->ResolveQueryData(m_pQueryHeap_, D3D12_QUERY_TYPE_TIMESTAMP, 0, m_TimerCount * 2, m_pQueryResourceGPU, 0);

		setGPUResourceState(pCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);

		if (ppQueryResourceGPUOut)
		{
			*ppQueryResourceGPUOut = m_pQueryResourceGPU;
		}
	}

	void D3D12Timer::setGPUResourceState(ID3D12GraphicsCommandList* pCommandList, D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES newState)
	{
		D3D12_RESOURCE_BARRIER barrierDesc = {};
		barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrierDesc.Transition.pResource = m_pQueryResourceGPU;
		barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrierDesc.Transition.StateBefore = prevState;
		barrierDesc.Transition.StateAfter = newState;

		pCommandList->ResourceBarrier(1, &barrierDesc);
	}

	GPUTimestampPair D3D12Timer::GetTimestampPair(UINT timestampPairIndex)
	{
		GPUTimestampPair p{};

		{
			GPUTimestampPair* mapMem = nullptr;
			D3D12_RANGE readRange{ sizeof(p) * timestampPairIndex, sizeof(p) * (timestampPairIndex + 1) };
			D3D12_RANGE writeRange{ 0, 0 };
			if (SUCCEEDED(m_pQueryResourceCPU->Map(0, &readRange, (void**)&mapMem)))
			{
				mapMem += timestampPairIndex;
				p = *mapMem;
				m_pQueryResourceCPU->Unmap(0, &writeRange);
			}
		}

		return p;
	}

	// Calcluate time and map memory to CPU.
	void D3D12Timer::CalculateTime()
	{
		// Copy to CPU.
		UINT64 timeStamps[2];
		{
			void* mappedResource;
			D3D12_RANGE readRange{ 0, sizeof(UINT64) * m_TimerCount * 2 };
			D3D12_RANGE writeRange{ 0, 0 };
			if (SUCCEEDED(m_pQueryResourceCPU->Map(0, &readRange, &mappedResource)))
			{
				memcpy(&timeStamps, mappedResource, sizeof(UINT64) * m_TimerCount * 2);
				m_pQueryResourceCPU->Unmap(0, &writeRange);
			}
		}

		m_BeginTime = timeStamps[0];
		m_EndTime = timeStamps[1];

		//			if (mBeginTime != 0) MessageBoxA(0, "ddd", "", 0);

		m_DeltaTime = m_EndTime - m_BeginTime;
	}

	// Get time from m_Start to m_Stop in nano seconds.
	UINT64 D3D12Timer::GetDeltaTime()
	{
		return m_DeltaTime;
	}

	UINT64 D3D12Timer::GetEndTime()
	{
		return m_EndTime;
	}

	UINT64 D3D12Timer::GetBeginTime()
	{
		return m_BeginTime;
	}

	// Whether timer is active.
	bool D3D12Timer::IsActive()
	{
		return m_Active;
	}
}