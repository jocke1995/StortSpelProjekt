#pragma once

namespace D3D12
{

	struct GPUTimestampPair
	{
		UINT64 Start;
		UINT64 Stop;
	};

	// D3D12 timer.
	class D3D12Timer {
	public:
		// Constructor.
		D3D12Timer();

		// Destructor.
		~D3D12Timer();

		HRESULT Init(ID3D12Device* pDevice, UINT numTimers);

		// Start timestamp.
		void Start(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndex);

		// Stop timestamp.
		void Stop(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndex);

		// Resolve query data. Write query to m_pDevice memory. Make sure to wait for query to finsih before resolving data.
		void ResolveQueryToCPU(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndex);
		void ResolveQueryToCPU(ID3D12GraphicsCommandList* pCommandList, UINT timestampPairIndexFirst, UINT timestampPairIndexLast);
		void ResolveQueryToGPU(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource** ppQueryResourceGPUOut);

		GPUTimestampPair GetTimestampPair(UINT timestampPairIndex);

		// Calcluate time and map memory to CPU.
		void CalculateTime();

		// Get time from m_Start to m_Stop in nano seconds.
		UINT64 GetDeltaTime();
		UINT64 GetEndTime();
		UINT64 GetBeginTime();

		// Whether timer is active.
		bool IsActive();

	private:
		void setGPUMemorytate(ID3D12GraphicsCommandList* pCommandList, D3D12_RESOURCE_STATES prevState, D3D12_RESOURCE_STATES newState);

		ID3D12Device* m_pDevice = nullptr;
		ID3D12QueryHeap* m_pQueryHeap_ = nullptr;
		ID3D12Resource* m_pQueryResourceCPU = nullptr;
		ID3D12Resource* m_pQueryResourceGPU = nullptr;
		bool m_Active = false;
		UINT64 m_DeltaTime = 0;
		UINT64 m_BeginTime = 0;
		UINT64 m_EndTime = 0;
		UINT m_TimerCount = 0;
	};
}