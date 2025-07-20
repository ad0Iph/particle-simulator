__kernel void calculateForces(
    __global float2* positions,
    __global float2* accelerations,
    __global float* charges,
    __global float* masses,
    float k,
    int count)
{
    int i = get_global_id(0);
    float2 acc = (float2)(0.0f, 0.0f);
    
    for (int j = 0; j < count; j++) {
        if (i == j) continue;
        
        float2 delta = positions[j] - positions[i];
        float distance = length(delta);
        float force = -k * charges[i] * charges[j] / (distance * distance + 0.01f);
        
        acc += force * normalize(delta) / masses[i];
    }
    
    accelerations[i] = acc;
}