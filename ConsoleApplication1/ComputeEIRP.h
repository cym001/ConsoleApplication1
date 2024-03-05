#ifndef COMPEIRP_H
#define COMPEIRP_H
#define DLL_API __declspec(dllexport)

using namespace std;

struct Result
{
	int INFO;
	double EIRP;
};

extern "C"
{
	DLL_API Result* init();

	DLL_API void ComputeEIRP(double EIRP0, double BW3dB, double Augment, \
		double Attenuation, double ThetaBeam, double PhiBeam, double ThetaTarget, double PhiTarget, Result* resultEIRP);

	DLL_API void final(Result* resultEIRP);
}

#endif
