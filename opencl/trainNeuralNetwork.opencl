typedef struct struct_neuralnetwork NeuralNetwork;
typedef struct struct_neuron Neuron;

struct struct_neuron
{
	unsigned uint flag;
	uint functionId;
	uint nPredecessors;
	uint* predecessorIds;
	float weight;
	float bais;
};

struct struct_neuralnetwork
{
	unsigned uint type;
	uint depth;
	uint nInput;
	uint nOutput;
	uint nLayers;
	uint* nLayer;
	uint n;
	float* charge;
	float* temp;
	Neuron* neurons;
};

/* LOCAL THREAD VARIABLE */
__constant float SQRT_2PI_F = 2.5066282746310005024157652848110452530069867406099383f;

/* ACTIVATION FUNCTIONS AND THERE DERIVATIVES */
float activationSigmoid(float x)
{
	return 1.0f / (1.0f + exp(-x));
}

float activationDDxSigmoid(float x)
{
	float q = 1.0f / (1.0f + exp(-x));
	return q * (1 - q);
}

float activationNormal(float x)
{
	return (1.0f / SQRT2PI_F) * exp(-x^2 / 2.0f);
}

float activationDDxNormal(float x)
{
	return -0.398942f * exp(2.0f / x^2.0f) * x;
}

float activationHyperbolicTangent(float x)
{
	float q = exp(2.0f * x);
	return (q - 1.0f) / (q + 1.0f);
}

float activationDDxHyperbolicTangent(float x)
{
	float q = exp(2.0f * x);
	float p = (q - 1.0f) / (q + 1.0f);
	return 1 - p^2;
}

float activationRectified(float x)
{
	return max(0.0f, x);
}

float activationDDxRectified(float x)
{
	/* note x should be undefined if zero */
	return x < 0.0f ? 0.0f : 1.0f ;
}

float activationCustom(float x)
{
	/*CUSTOM CODE*/
	return x;
}

float activationDDxCustom(float x)
{
	/*CUSTOM CODE*/
	return 1.0f;
}

/* FUNCTIONS */
void backpropagation(__global *NeuralNetwork oNetwork)
{

}

void forwardpropagation(__global *NeuralNetwork oNetwork, uint iInput)
{
	Neuron = NeuralNetwork->neurons[iInput]
}

/*PROGRAM ENTRY*/
__kernel void entry(__global read_only uchar4* lpSamples, __global *NeuralNetwork oNetwork)
{
	/*DO NOTHING*/
}