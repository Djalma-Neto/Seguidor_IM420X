#include "pid.h"
#include "math.h"
#include "stdlib.h"
float fKpE, fKiE, fKdE, fTsE, fOutminE, fOutmaxE;
float fKpD, fKiD, fKdD, fTsD, fOutminD, fOutmaxD;

float fErrorP_E, fErrorI_E, fErrorD_E;
float fErrorP_D,fErrorI_D,fErrorD_D;

void PID_init_D(sPID_D *par) {
	fKpD = par->fKpD;
	fKiD = par->fKiD;
	fKdD = par->fKdD;
	fTsD = par->fTsD;
	fOutminD = par->fOutminD;
	fOutmaxD = par->fOutmaxD;
}
void PID_init_E(sPID_E *par) {
	fKpE = par->fKpE;
	fKiE = par->fKiE;
	fKdE = par->fKdE;
	fTsE = par->fTsE;
	fOutminE = par->fOutminE;
	fOutmaxE = par->fOutmaxE;
}

float PID_E(float input, float set_point) {
	// Proportional
	float fPout, fIout, fDout, fOut, fErrorP_E;

	fErrorP_E = (set_point - input);
	fPout = fKpE * fErrorP_E;

	// Integral
	fErrorI_E += fErrorP_E * fTsE;
	fIout = fKiE * fErrorI_E;

	// Derivative
	fErrorD_E = (fErrorP_E-fErrorD_E)/fTsE;
	fDout = fKdE * fErrorD_E;

	fOut = fPout + fIout + fDout;

	// Saturation

	if (fOut > fOutmaxE) {
		fOut = fOutmaxE;
	}
	if (fOut < fOutminE) {
		fOut = fOutminE;
	}
	return fOut;
}
float PID_D(float input, float set_point) {
	// Proportional
	float fPout, fIout, fDout, fOut, fErrorP_D;

	fErrorP_D = (set_point - input);
	fPout = fKpD * fErrorP_D;

	// Integral
	fErrorI_D += fErrorP_D * fTsD;
	fIout = fKiD * fErrorI_D;

	// Derivative
	fErrorD_D = (fErrorP_D-fErrorD_D)/fTsD;
	fDout = fKdD * fErrorD_D;

	fOut = fPout + fIout + fDout;

	// Saturation

	if (fOut > fOutmaxD) {
		fOut = fOutmaxD;
	}else if (fOut < fOutminD) {
		fOut = fOutminD;
	}
	return fOut;
}
