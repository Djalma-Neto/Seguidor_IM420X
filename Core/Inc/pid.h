#ifndef PID_H_
#define PID_H_

typedef struct {
	float fKpD;
	float fKiD;
	float fKdD;
	float fTsD;
	float fOutminD;
	float fOutmaxD;

}sPID_D;

typedef struct {
	float fKpE;
	float fKiE;
	float fKdE;
	float fTsE;
	float fOutminE;
	float fOutmaxE;

}sPID_E;

void PID_init_E(sPID_E *par);
void PID_init_D(sPID_D *par);
float PID_E(float input, float set_point);
float PID_D(float input, float set_point);

#endif /* PID_H_ */
