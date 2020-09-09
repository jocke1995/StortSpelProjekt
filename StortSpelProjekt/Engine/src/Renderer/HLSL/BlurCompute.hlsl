
[numthreads(3, 1, 1)]
void CS_main(uint3 DTid : SV_DispatchThreadID)
{
	int threadId = DTid.x;

	if (threadId <= 3)
	{
		//materialColor0[0].rgba[threadId] *= 10.0f;
		//materialColor0[0].rgba[threadId] = abs(sin(materialColor0[0].rgba[threadId]));
	}
}