cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer entityConst : register (b1)
{
	matrix entityTransform;
	matrix entityTransformRotation;
};
