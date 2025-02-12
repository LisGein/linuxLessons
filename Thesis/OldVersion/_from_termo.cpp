STRICT
#define VC_EXTRALEAN	// speed up build process
#include "stdafx.h"
#include <math.h>
#include "ThermoCalc.h"
#include "_from_termo.h"

void empty_IPAGE();
void _GET_CALBR(const char *,int);
void _GET_CALBR(int *);
void _GET_CALBR(float *);
int nCALBR_STREAM,nCALBR_STREAM_size;
/*char cCALBR_STREAM[]={"\ALFA 1 0.65\PK   1 1\GGOR 1 1.0\
GOR  1 6 30.48 C    -1000\GOK  1 1.00\OK   1 8 39.62 O2   3.2 EOD "};*/

char cCALBR_STREAM[]={"\
ALFA 4 0.35 0.4 0.5 1.0\
PK   1 0.1\
GGOR 2 1.0 0.0\
GOR  4 1 62.4989 6 41.045 7 0.68 8 27.155 WOOD    -5600\
GOR  2 1 2 8 1 WATER   -15000\
GOK  1 1.00\
OK   3 6 0.01 7 54.61 8 14.681 AIR     3.2 EOD "};

/*char cCALBR_STREAM[]={"\ALFA 1 0.65\PK   1 0.1\GGOR 1 1.0\
GOR  3 1 62.4989 6 41.045 8 27.155 W___      -5600\
GOK  1 1.00\OK   2 6 0.01 8 14.681 A___      3.2 EOD "};*/
char cReserv[500],c;

extern float P[MaxElemGasFromDB];	// ��� �������� �������

//TERMO
//DCL BEG CHAR (2) INIT ('  ');
char BEG[2];

//DCL (N,M,NG,RK,JE,CT) BIN FIXED;
int CT=0;

//DCL (... , NAMO)(25) CHAR (7);
char NAMG[25][7],NAMO[25][7];

//DCL (GO,GG)(25) DEC;
float GO[25],GG[25];

float MU[100];

//DCL (T,T1,T2,TK,CK,PP,P1,P2,PPK,PPA,PKP,LPI,PI,H,R,R0,MIN,ZK,MS,MG,BT,W,PYD,PYDP,CPR,
//IS,SS,CP3,IS1,CP31,AA,E,HK,RC,RR,BTMIN,T3AM,MGK,ZKK,DA,Q,FA,FX,FY,PX,PY,
//E1,E2,KAPPA) DEC FLOAT(7);
/*double T,T1,T2,TK,CK,PP,P1,P2,PPK,PPA,PKP,LPI,PI,H,R0,MIN,ZK,MS,MG,
BT,CPR,IS,SS,CP3,IS1,CP31,AA,E,HK,RC,RR,BTMIN,T3AM,MGK,ZKK,DA,Q,FA,
FX,FY,PX,PY,E1,E2,KAPPA;*/
float T3AM,KAPPA,PPA,FA,FX,FY,PX,PY,RR;double TK,PPK,PKP,LPI,PI,MIN,ZK,MG,
BT,CP3,IS1,CP31,AA,HK,RC,BTMIN,MGK,ZKK,DA,
E1,E2;
// ZK=ZK+MK[I-NG]*P[I];		���� ����������,
// MS=1000/A;					������������ ����� �����
// MG=MS*(1-ZK);				������������ ����� ����
// PPK							�������� � ��
// PP								������� ��������

//DCL (F,F1,F2,FV,FF,FE,FT,FHAC,FSH,FIS,FRY,F3C,FP3,FPP,F0T,CKACH,KAMEPA,FKP,FS,FB) BIT(1);
//BOOL F,F1,F2,F3,F4,F5,F6,F7,F8,FV,FF,FE,FT,FHAC,FSH,FIS,FRY,F3C,FP3,FPP,F0T,CKACH,KAMEPA,FKP,FS,FB;
BOOL F,F1,F2,F3,F4,F5,F6,F7,F8,FF,FE,FHAC,FSH,FRY,FS,FKP;

//DCL (AK(300)) DEC(10);
//float AK[300];
//DCL ST(80,7) DEC (10)
float ST[80][7];
float _ST[12];

//DCL ((PK,PA,ALFA)(25),G(100),O(100),IO,IG) DEC;
float PK[25],PA[25],ALFA[25],G[100],O[100],IO,IG;

//DCL (NPA,NPK,NA, ... HAC(5) ... ) BIN FIXED;
//int NPA,NPK,NA,NKG,NKO,HAC[5],SHY[50];
int NPA,NPK,NA,NKG,NKO,SHY[50];

//DCL ( ... , A2(25) DEC FIXED(3), ... )
float A2[25];

//double	XF1[500];//double	XF2[500];

int	I,J,J1,I2,X,Y,I1;

// ����������� ������������ �� ���������
long IC;
long IPK,IPA,IA;
long CC;
long DR;
int BP=1; //������ ����� ��������
int AP;
// ��������� �������������
//FF=FE=FT=FV=FALSE;
//ST=0;
//double R0=1.98719;double E1=5E-5;double E2=5E-4;
//int M=2;int N=3;int NG=1;
//int JE=3;int CT=1;int AP=0;int CC=0; int HAC[5]={0,0,0,0,0};
//int IA=1;int IPK=1;int IPA=0;

#pragma warning (disable : 4244)
// DCL (ENT(N),S(N),CP(N),P(N),PR(N),XR(N),A5(M),A6(M),LP(M),
// B(M,M)) DEC(10);
double	ENT[120]={0,};
double	S[120]={0,};
double	CP[120]={0,};
//double	P[120]={0,};
int		A5[10]={0,};
double	A6[nSize_A6]={0,};
double	LP[10]={0,};
double	B[10][10]={{0},{0}};
// DCL (MK(MAX(1,N-NG)) DEC; MK=0,0;
double	*MK=0;	// 'MK' ������������ ����� ����. ���.
						
void init_var_of_TERMO1 (const double extM[],int &JE,BOOL &FT,BOOL &FV,double &R0,int HAC[]) {
// ��������� �������������
	//BEG[0]='V';BEG[1]='R';// ��������� VR - ���� ������

	FF=FE=FT=FV=FALSE;
//	ST=0;
	R0=(float)1.98719;		// � ��������
	//R0=1.98719*4.1868;		// � �������

	E1=5E-5;
	E2=5E-4;

//	E1=5E-9;
//	E2=5E-7;
//	int M=2;int N=3;int NG=1;
	JE=3;

	for (int i=1;i<=100;i++)
		MU[i]=extM[i];

	CT=1;
	AP=0;
	CC=0;
	HAC[0]=HAC[1]=HAC[2]=HAC[3]=HAC[4]=0;
	return;
};

void init_var_of_TERMO2 () {
	IA=0;// �������� � ����� 1
	IPK=1;
	IPA=0;
	return;
};

void init_var_of_TERMO3 (BOOL &FB) {
//	FB=FKR=FALSE;					���������� ������
	FB=FKP=FALSE;
	PKP=0.0;
	return;
};

void init_var_of_TERMO4 (int &N, int &NG) {
//	DCL (MK(MAX(1,N-NG)) DEC;
//	MK=0.0;
	if (!MK)
		if ((N-NG)!=0) {
			MK=new double [N-NG+1];
			for (int i=0;i<N-NG+1;i++)
				MK[i]=0.0;
		}
		else {
			MK=new double [1+1];
			MK[0]=MK[1]=0.0;
		}
	else
		if ((N-NG)!=0) {
			delete [] MK;
			MK=new double [N-NG+1];
			for (int i=0;i<N-NG+1;i++)
				MK[i]=0.0;
		}
		else {
			MK[0]=MK[1]=0.0;
		}


	return;
};

//void PABHPAC (float XF1[],float XF2[], int M, int N, int NG) {
void PABHPAC (float XF1[],float XF2[], int M, int N, int NG,
	BOOL &FPP,BOOL &F3C,BOOL &FB,BOOL &FP3,float &CK,double &R0,
	float &MS,double &CPR,double &IS,float &T,double &SS,
	float &E,float &PP,int HAC[]) {

	float _E;
	int i;		// �� �� ������������� ����������� �������� ������
				// ����� �����
// DCL (K,D,A,Q,PI,LPI,A1,A2,AA,FL,MM,R,YI,C,DK) DEC (6);
	double K=0,D,A,Q,PI,LPI,A1,A2,AA=0,FL,MM,R,YI,C,DK;

	FL=1;
	YI=1;
	// ����������� ������������ �� ���������
	int IT=0;

	if (T>T3AM) {
		F3C = FALSE;
		FP3 = FALSE;
	}

BEGIN:

	if (FHAC)
		FB = FALSE;

	DR = 1;
	I1 = 1;
	F = TRUE;
	YI = -YI;
	A = 0;

	// 'A' ����������� ���������� ������ ����� � ��
	// ??? 'A' ����������� ���������� ����� � ���
	for (I=1;I<=N;I++) {
		if ((XF1[I]>0)&&(P[I]>0))
			A=A+P[I];		
	}

	//	if (A<=PP*(1E-18))
	//		goto LA;
	if (A>PP*(1E-18)) {

		AA=log(PP/A);	// 'AA' �������� �� ���� ����
		if (F3C)
			goto TD;

		F1 = TRUE;
		for (I=1;I<=N;I++) {
			A2=XF2[I];						// ??? ������ -XF2[]
			if (A2<0) {
				if ((I<=NG)&&(P[I]>1E-17))
					F1=FALSE;
				//goto LK;	//My code
				if (I==HAC[DR]) DR=DR+1;
				continue;
			}

			A1=XF1[I];
			PI=P[I];
			if ((A1>0)&&((PI/A)<0.1E-5)&&(FB)){//'A'���-�� ����� � ���
				//goto LK;	//My code
				if (I==HAC[DR]) DR=DR+1;
				continue;
			}

			if (A1<0)
				LPI=0;
			else
				if (PI<0)
					LPI=PI;
				else
					LPI=log(PI);

			if (A1<0)
				Q=0;
			else
				Q=-(AA+LPI);			// Q=-(log(PP/A)+log(PI))
			K=(S[I]-FL*ENT[I]/T)/R0;// K= ������ ������ ����� ��������� �����������

			//A6 = 0
			for (i=0;i<=nSize_A6;i++)
				A6[i] = 0;
			X=1;	Y=1;
			int nIndexInElementarSubst;
			for (I1=1;I1<=4;I1++) {
				J=floor(fabs(A1)/X)-floor(fabs(A1)/(X*100))*100;
				if (J==0)	break;
				J1=floor(A2/Y)-floor(A2/(Y*10))*10;
				nIndexInElementarSubst=1;
				while (XF1[nIndexInElementarSubst]!=J)
					nIndexInElementarSubst++;
				// ��� �� 'nIndexSubst' �������� ������ ����������.���.��.(�����) �� 'XF1[]'
				// ����� ������� ��� ������ � 'I'-��� ����������
				// ���� �� ������
				for (I2=1;I2<=M;I2++) {//'J1'� ����� ������������� ������������ ��. � ����������
					A6[I2]=A6[I2]+J1*B[I2][nIndexInElementarSubst];//'J1' ����������� ��� ����� 'I1' � ���������� 'I'
					if (fabs(A6[I2])<0.1E-7)  // ���� 'I2' ����� 'nIndex', �� 'B[I2][nIndexSubst]' ����� 1 �� ������ ��������
						A6[I2]=0;
				}
			// B[1..M][] ���� �� ������� - I2 � [1..M],
			// B[][J] � �������� nIndexSubst
			// ������� A6[]=0, ����� � A6[0..M](�� I2) �����������
			// ��� ������� �����.'J1'��� ����� 'I1'(0..4) � ����������
			// A6[I2] += J1_f(I1)*B[I2][nIndexSubst_f(I1)]
			// ������ 'I1'�������� 'nIndexSubst' ����� ������, 
			// �.�. ���������� ������� �� ������ �������
				X=X*100;
				Y=Y*10;
			}
			MM=1;
			for (I1=1;I1<=M;I1++) {
				_E=A6[I1];
				if (_E==0)	continue;
				//J=A5[I1];
				if ((A1<0)||(PI>=0.01*P[(A5[I1])]))	//if ((A1<0)||(PI>=0.01*P[J]))
					MM=0;
				K=K+_E*(ENT[(A5[I1])]*FL/T-S[(A5[I1])])/R0;	//K = K1[I] + _E*K2[(A5[1..M])]
				if (XF1[(A5[I1])]<0)
					continue;
				Q=Q+_E*(log(P[A5[I1]])+AA);	// Q=Q+_E*(LP[I1]+AA);
				//Q=_E*(log(PP/A)+log(P[A5[I1]]))-(log(PP/A)+log(P[I]))
			}

			C=K+Q;
			if (I!=HAC[DR])
				if ((A1<0)&&(PI<0.1E-18)&&(C<0)) {
					//goto L2;	//My code
					P[I]=PI; if (I==HAC[DR]) DR=DR+1;
					continue;
				}

			if (MM==0)
				_E=E1;	//E1=5E-5;
			else
				_E=E2;	//E2=5E-4;
			if ((fabs(C))<_E) {
			//goto L2;	//My code
				P[I]=PI; if (I==HAC[DR]) DR=DR+1;
				continue;	
			}

			F = FALSE;
			//	if (���������� ������) ok
			if ((I!=HAC[DR])&&(C<=0))
				MIN=PI;
			else
				MIN=0.1E5;

			if (A1<0)
				D=0;
			else
				D=1/PI;

			for (I1=1;I1<=M;I1++) {
				R=A6[I1];
				if (R==0)
					continue;
				//J=A5[I1];
				if ((R*C)>=0) {
					_E=P[(A5[I1])]/fabs(R);		//_E=P[J]/fabs(R);
					if (_E<MIN)
						MIN=_E;
				}
				if ((MM==0)&&(XF1[(A5[I1])]>0))	//if ((MM==0)&&(XF1[J]>0))
					D=D+R*R/P[(A5[I1])];		//D=D+R*R/P[J];
			}

			float reduce_MIN =(float) 0.9;
			//if (MM==0)//	goto L5;
			if (MM!=0) {
				//if (YI>0)	//	goto L2;
				if (YI <=0) {
					LPI=LPI+C;
					//if ((PI<0)&&(C<0)) { //PI=LPI; //goto L2; //}
					if (!((PI<0)&&(C<0))) {
						if (LPI>40)
							_E=2*MIN;
						else
							_E=exp(LPI);

						if (PI<0)
							DK=_E;
						else
							DK=_E-PI;
						if (DK>MIN) {
							//	goto L8;
							//L8:
							if (fabs(DK)>=MIN)
								DK=reduce_MIN*MIN*SIGN(C);
								//if (C>=0)	DK=0.9*MIN*1;
								//else		DK=0.9*MIN*(-1);

							PI=PI+DK;						// ����������� + ��� *
						}
						else {
							if (LPI<-42)
								PI=LPI;		// ����� ����������� ������������� LPI
							else
								PI=_E;
							//goto L3;
						}
					}
					else {
						PI=LPI;
						//goto L2;	//My code
						P[I]=PI; if (I==HAC[DR]) DR=DR+1;
						continue;
					}
				}
				else {
					//goto L2;	//My code
					P[I]=PI; if (I==HAC[DR]) DR=DR+1;
					continue;
				}
			}
			else {
				//L5:
				if (D==0)	DK=MIN/reduce_MIN;	// DK=10*MIN/9;
				else		DK=C/D;
				//L8:
				if (fabs(DK)>=MIN)
					//if (C>=0)	DK=0.9*MIN*1;
					//else		DK=0.9*MIN*(-1);
					DK=reduce_MIN*MIN*SIGN(C);
				PI=PI+DK;						// ����������� + ��� *
			}

			//L3:
			MIN=0.1E18;
			for (I1=1;I1<=M;I1++) {
				R=A6[I1];
				if (R==0)
					continue;
				//J=A5[I1];
				P[(A5[I1])]=P[(A5[I1])]-R*DK;
				if (XF1[(A5[I1])]>0)
					LP[I1]=log(P[(A5[I1])]);
				if (P[(A5[I1])]<MIN){
					I2=I1;
					MIN=P[(A5[I1])];
				}
			}

			if (PI>=MIN) {
				//J=A5[I2];
				XF2[(A5[I2])]=fabs(XF2[(A5[I2])]);
				A5[I2]=I;
				XF2[I]=-fabs(XF2[I]);
				LP[I2]=log(PI);		//natural
				F1 = FALSE;

				MIN=1/A6[I2];	// 1<=I2<=M
				for (I1=1;I1<=M;I1++) {
					_E=B[I2][I1];
					if (_E==0)	//goto L6;
						continue;
					for (J1=1;J1<=M;J1++) {
						B[J1][I1]+=_E*A6[J1]*(-MIN);
						if (fabs(B[J1][I1])<0.1E-7)
							B[J1][I1]=0;
					}
					B[I2][I1]=_E*MIN;//A6[I2]
					if (fabs(B[I2][I1])<0.1E-7)
						B[I2][I1]=0;
					//L6:
				}
			}

			P[I]=PI;		//L2:
			if (I==HAC[DR])	//LK:
				DR=DR+1;
		}
	}

	if (F1&&(A<1E-14)) {
		MS=0;
		MG=0;
		ZK=1;
		FPP = TRUE;
		//goto LZ;
	}
	else {

		//if (���������� ������) ?F ok
		if (!F) {
			IT=IT+1;
			goto BEGIN;
		}

		if (FB) {
			FB = FALSE;
			goto BEGIN;
		}
		else
			FB = TRUE;
TD:// ������� ���� ���� F3C==TRUE;
		//	if(F1)
		//		goto LZ;
		if(!F1) {
			//if (���������� ������) FL? ok
			//	if(FL!=1)
			//		goto LZ;
			if(FL==1) {
				ZK=0;
				// �������� ������ ���� ����������	
				for (I=NG+1;I<=N;I++) {
					ZK=ZK+MK[I-NG]*P[I];//'ZK'���� �����������,'MK' ������������ ����� ����. ���.
				}

				MS=1000/A;			// ������������� ����� �����
				ZK=ZK/1000;
				MG=MS*(1-ZK);		// ������������ ����� ����
			}
		}
	}
	//LZ:

	if (FL==1)
		SS=-R0*A*AA;
	IS1=0;
	CP31=0;

	for (I=1;I<=N;I++) {
		PI=P[I];
		if (PI<=0)
			//goto LI;
			continue;
		IS1=IS1+ENT[I]*PI;
		CP31=CP31+CP[I]*PI;
		//if (���������� ������) FL? ok
		if(FL!=1)
			//goto LI;
			continue;
		//������ FL==1
		if(XF1[I]>0)
			SS=SS+PI*(S[I]-R0*log(PI));
		else
			SS=SS+PI*S[I];
		//LI:
	}

	if (FHAC) {
		E=0;
		for (I=1;I<=DR-1;I++) {
			I1=HAC[I];
			if (P[I1]>0) {
				IS1=IS1-ENT[I1]*P[I1];
				CP31=CP31-CP[I1]*P[I1];
				if (FL==1)
					SS=SS-S[I1]*P[I1];
			}
			E=E+MK[I1-NG]*P[I1];
		}
		E=1000/(1000-E);
		IS1=IS1*E;
		CP31=CP31*E;
		if (FL==1)
			SS=SS*E;
		MS=MS/E;
		ZK=1-(1-ZK)*E;
		MG=MS*(1-ZK);
	}

	IS1=IS1/1000;
	CP31=CP31/1000;

	if (FL==1) {
		//	if (���������� ������) CK? ok
		if (CK!=0)
			BT=CK*P[IC]/1000;
		else
			BT=0;
	}

	if (FL==1) {
		IS=IS1;
		SS=SS/1000;
		CP3=CP31;
		if (F3C||FHAC) {
			CPR=CP3;
			//goto MEN;
			//MEN:
			E=BT;
			return;
		}

		//	if (���������� ������) ?F1 ok
		if (!F1) {
			FL=1-(0.1/T);
			goto BEGIN;
		}
	}

	CPR=CP31+(IS1-IS)/0.1;
	E=BT;
	return;
}
//double	MU[99];//double	MC[120];
void FIRST (float XF1[500],float XF2[],float MC[], int M, int N, int NG, BOOL &FPP, float P[]) {
	long A1,A2;

	//int aa = sizeof(A5);
	memset(A5,0,sizeof(A5));
	FPP=FALSE;

	for(I=M+1;I<=N;I++){
		A1=fabs(XF1[I]);
		XF2[I]=fabs(XF2[I]);
		A2=XF2[I];
		MIN=0;
		X=1;
		Y=1;
		for (I1=1;I1<=4;I1++){
			J=floor(A1/X)-floor(A1/(X*100))*100;
			if (J==0) 
				goto LL;
			J1=floor(A2/Y)-floor(A2/(Y*10))*10;

			for (I2=1;I2<=M;I2++){
				if (J == XF1[I2])
					A5[I2] = A5[I2]+J1;
			}

			if (I>NG)
				MIN += MU[J]*J1;
			X=X*100;
			Y=Y*10;
		}
LL:
		if (I>NG)
			MK[I-NG]=MIN;
	}
	// ���� ���������� ���-�� ������ � �����������

	MIN=1E17;
	for (I=1;I<=M;I++) {
		J=XF1[I];
		if ( (MC[J]/(A5[I]+1))<MIN )
			MIN=MC[J]/(A5[I]+1);
	}
	for (I=M+1;I<=N;I++)
		P[I]=MIN;
// ��� �������� ����� ������-�� ������������ �������� 
// � ��������� ���������� ��� ������� �����������

	for (I=1;I<=M;I++) {
		J=XF1[I];
		P[I]=MC[J]-MIN*A5[I];
		//P[I]=MC[XF1[I]]-MIN*A5[I];
		XF2[I]=-fabs(XF2[I]);
		A5[I]=I;
		LP[I]=log(P[I]);	// ����������� ��������
	}

	// ����������� ������� ������� ��������� �������
	for (I=1;I<=M;I++) {
		for (J=1;J<=M;J++) {
			if (I==J)
				B[I][J]=1;
			else
				B[I][J]=0;
		}
	}
	return;
};

BOOL _NewAlfa(float MC[],int &M,int &JE,BOOL &F0T,BOOL &FPP,BOOL &FIS,float &H,float &T) {
	if ((IA==0)&&(NA==0)) { IA=1; H=0; IO=0; } 
	else {	// goto L4;
		IA=IA+1;
		if (IA>NA) { JE=1; goto _LK; }// ���� �� ���������� ��� �������� �����
		H=ALFA[IA]*KAPPA;
		if ((BP==1)&&(CC!=0))
			if (CC>=46)	empty_IPAGE();	//call IPAGE;
			else {	CC=CC+3;	}//PUT EDIT();
	}//L4://
	FPP=TRUE;
	F0T=FALSE;
	for (I=1;I<=99;I++) {
		if (NA==0)	T=G[I];
		else			T=(G[I]+H*O[I])/(1+H);
		if ( ((T==0)&&(MC[I]!=0))||((T!=0)&&(MC[I]==0)) )
			F0T=TRUE;
		MC[I]=T;
	}
	if ((IA==1)&&(FHAC||FSH)) F0T=TRUE;
	M=0;
	for (I=1;I<=99;I++)
		if (MC[I]!=0) M=M+1;
	if (!FIS) H=(IG+H*IO)/(1+H);
	return TRUE;
_LK:
	return FALSE;
};

void _NewT_T1_T2(float &T, float &T1, float &T2) {
	//My code
	T1 = 0;
	T2 = 3000;
	T = (float)1084.8;
};

void _NewT_HK_TK(float &H, float &T, float &T1, float &T2) {
	T2=TK;
	T=TK;
	T1=0;
	H=HK;
};

BOOL MENJE (float MC[],int &M,int &JE,BOOL &F0T,BOOL &FPP,BOOL &CKACH,
		BOOL &FIS,BOOL &KAMEPA,BOOL &FT,BOOL &F3C,
		BOOL &FV,BOOL &FP3,float &H,float &P1,
		double &IS,float &T,double &SS,float &T2,float &T1,float &PP,float &P2) 
{
	F0T=CKACH=FALSE;
	// ... ��������� ������ ���������
	if (FKP) {					// goto LKP;	// LKP:
//		_Func_decPPA_PPequPPA(T1,PP);
		goto LK;					//exit
	}//	else { if (PKP!=0) goto L1; }

	if (( (!FIS)&&(IPA!=0) )||( (FKP == 0)&&(PKP!=0) )) {
		//L1_SectionPressures:
		if ((!FF)||(PY==PX)) {	// if (FF&&(PY!=PX)) goto LR;
			IPA=IPA+1;
			if (IPA>NPA) {
				IPA=0;				// if (FIS) goto L2;
				goto L3;
			}
//			_Func_if_FF (PP);		// ��������� ��������� �������� ������ �����
//					if (FE)
//					PPA=PPK/PA[IPA];
		}
		else { //LR:				// ��������� ��������� ������� �����
//			_Func_if_FF__PX_PY_FX_FY_PPA(PP);
		} //LL:
//		_Func__T2_PPA_PP_P1_P2_T_FP3_T1(FT,F3C,FP3,T3AM,P1,T,T2,T1,PP,P2);//LL:
 		goto LK;
	}

	if (IPK==0)
		goto L2;

	if ((!KAMEPA)||(FIS))
		goto L3;
	else { // if ((KAMEPA!=0)&&(FIS==FALSE))
//		if(!_Func_if_FV_NewT_PP_FlagF3C(KAMEPA,FKP,F3C,FV,FP3,H,T3AM,IS,T,SS,T2,T1,PP)) {
//			_Func_decPPA_PPequPPA(T1, PP);
			goto LK;
//		}
//		else
			goto L3;
	}// end if...else: if ((!KAMEPA)||FIS) -> goto LK; //exit

L2:
		if (!_NewAlfa(MC, M, JE, F0T, FPP, FIS, H, T))
			goto LK;
L3:
		IPK=IPK+1;
		while (IPK>NPK) {
			IPK=0;
			if (!FIS)
				if (!_NewAlfa(MC, M, JE, F0T, FPP, FIS, H, T))
					goto LK;
			IPK=IPK+1;
		};

//		_NewPP_KAMEPA(KAMEPA,F3C,FP3,PP);
		if (!FIS) {
			if (IPK==1)
//				_NewT_T1_T2(T, T1, T2);
				;
			else
//				_NewT_HK_TK(H, T, T1, T2);
				;
			JE=2;
			goto LK;			//exit
		} else {
			// ����� �� �������� 'goto L1_SectionPressures' �������� ���� 'FIS == TRUE'
			// ���� ��� ����� FIS!=0 �� ����� �� �� ������ TRUE
			if (IPK!=1) {		//if (FIS)
				JE=3;
				goto LK;			//exit
			} else {				//if (IPK==1) {
				// goto L1_SectionPressures; ��������� ����� 'goto L1_SectionPressures'
				if ( (!FF)||(PY==PX) ) {
					if (IPA<=NPA) {	// ���� FIS == TRUE
						IPA=IPA+1;
						JE=2;
						T=PA[IPA];
						goto LK;
					}
					if (IPA>NPA) {		// ���� FIS == TRUE
						IPA=0;
						goto L2;
					}
				} else {	// if ((FF)&&(PY!=PX))// ��������� ����� 'goto L1_SectionPressures'
					// ��������� ��������� ������� �����
//					_Func_if_FF__PX_PY_FX_FY_PPA(PP);
//					_Func__T2_PPA_PP_P1_P2_T_FP3_T1(FT,F3C,FP3,T3AM,P1,T,T2,T1,PP,P2);
 					goto LK;
				}
			}
		}
LK:
	return TRUE;
};

BOOL CALBR (BOOL &FIS,BOOL &FT,BOOL &FV,int HAC[]) {
	// ����������� ������������ �� ���������
	int NK=0,NHAC,NSHY;

	int I,i,i1,i2,s;// �� �� ������������� ����������� �������� ������
					// ����� �����
	// My code
	//T3AM=1500;
	T3AM=0;
// DCL
	int V[99]={1,0,1,2,3,4,0,-2,-1};					//char CHAR[75];
	char KOH;//	char ROL[80];
	CString REJ,REJ1;			//	char REJ[4],REJ1[4];
	int NI=0,N=0,NM=0,INK=0;							//	float A2[25];
	float KOK[25][20],KGOR[25][20],AK[25][20];
	int SOK[25][20],SGOR[25][20],AS[25][20];
	float /*B,*/K1,K2,A1[25],/*GU[25],*/EO[25],EG[25],ENT[25],M[100];
	char NAMA[25][7];
LI:
	if ((BEG[0]=='V')&&(BEG[1]=='R')) {/*...*/;}	// ��������� VR - ���� ������

	REJ="    ";					// REJ[0]=REJ[1]=REJ[2]=REJ[3]=' ';
	KOH=' ';
	N=0;

LIN:
	if (KOH==' ')
		N=0;
	REJ1=REJ;					//	memcpy(REJ1,REJ,4*sizeof(char));
	if ((BEG[0]=='V')&&(BEG[1]=='R')) // ��������� VR - ���� ������
		{/*...*/MV:;}
	else {
		//GET EDIT(REJ,CHAR,KOH) (A(4),A(75),A(1)) COPY;
		_GET_CALBR((LPCTSTR)REJ,4);
		//_GET_CALBR(CHAR,75);
		//_GET_CALBR(&KOH,1);
	}

//LI1:
	if (REJ=="VR  ")
		goto MV;
	if (REJ=="EOD ")
		goto CBR;
	if ((REJ=="EOJ ")&&(BP==-1)) {
		empty_IPAGE();
//		STOP
		return FALSE;
	}

	memset (A1,0,25*sizeof(int));	// A1=0;
	NM=0;
	if ((N!=0)&&(REJ!=REJ1))
		goto ERR;
//LR:
	if (REJ=="S   ")	// �� �� �������, �� �� ��������� - ��������������� ���������
		FS=TRUE;
	if (REJ=="PRINT")
		BP=1;
	if ((FS)&&(BP==1))
		goto LIN;

	if (REJ=="ALFA")
		NM=1;

	if (REJ=="JS  ") {
		NM=2;
		FIS=TRUE;
	}

	if (REJ=="V   ") {
		NM=2;
		FV=TRUE;
		FIS=FALSE;
	}

	if (REJ=="PK  ") {
		NM=2;
		FIS=FV=FALSE;
	}

// �������� �������� � �������� ����� 
// ��� ������� ��������� �� ������ ����������� ������ � ��������
	if (REJ=="PA  ") {
		FF=FE=FT=FALSE;
		NM=3;
	}

// �������� ���������� �������� �� ��������� � ��������� � ��. �������
	if (REJ=="FA  ") {
		NM=3;
		FF=TRUE;
		FT=FE=FALSE;
	}

// ???
	if (REJ=="EPS ") {
		NM=3;
		FE=TRUE;
		FF=FT=FALSE;
	}

	if (REJ=="TA  ") {
		NM=3;
		FT=TRUE;					// ������ ���������� �� ����������. ??? ������������
		FF=FE=FALSE;
	}

	if (REJ=="GGOR")
		NM=4;

	if (REJ=="GOK ")
		NM=6;

	if ((NM==4)||(NM==6)) {			// ��������� �������� ����� �����������
		INK=0;
//		AS=0;
		memset (AS,0,25*20*sizeof(int));
//		AK=0;
		memset (AK,0,25*20*sizeof(int));
//		ENT=0;
		memset (ENT,0,25*sizeof(int));
//		NAMA=' ';
		memset (NAMA,0,25*7*sizeof(char));
	}

	if (REJ=="GOR ")
		NM=5;

	if (REJ=="OK  ")
		NM=7;

//	if (REJ=="TZAM")			Warning! L10 is undefined label
//		NM=10;

	if (REJ=="HAC ")
		NM=8;

	if (REJ=="SHY1")
		NM=9;

	if (REJ=="SHY2") {
		NM=9;
		FRY=TRUE;
	}

	if (NM==0)
		goto ERR;
// ON ERROR GOTO ERR;				����������� �������� ON ERROR

	if ((NM==5)||(NM==7)) {			// ��������� ������� �����������
		INK=INK+1;

//		GET STRING (CHAR)LIST(NI,(AS(INK,I),AK(INK,I)DO I=1 TO NI),
//			NAMA(INK),ENT(INK));
		_GET_CALBR(&NI);
	
		for (int I=1;I<=NI;I++) {
			_GET_CALBR(&AS[INK][I]);	// ������ �� �. ���������
			_GET_CALBR(&AK[INK][I]);	// ������������ �.��.
		}

		_GET_CALBR(NAMA[INK],7);
		_GET_CALBR(&ENT[INK]);

		if (INK!=NK)	// Warning unknown
			goto LIN;
		else {
//			goto LP(NM);
			switch (NM) {
				case 1:	goto L1;
				case 2:	goto L2;
				case 3:	goto L3;
				case 4:	goto L4;
				case 5:	goto L5;
				case 6:	goto L6;
				case 7:	goto L7;
				case 8:	goto L8;
				case 9:	goto L9;
				default:	;
			}
		}
	}

//		GET STRING(CHAR)LIST(NI,(A1(I) DO I=N+1 TO N+NI));
	_GET_CALBR(&NI);
	for (I=N+1;I<=N+NI;I++) {
		_GET_CALBR(&A1[I]);
	}


	N=N+NI;
	if (N==0)
//			PUT SKIP LIST('HET ������ ��� ���������',CHAR);
		N=N;

	if (KOH=='*')
		goto LIN;
	else {
		KOH=' ';
//			goto LP(NM);
		switch (NM) {
			case 1:	goto L1;
			case 2:	goto L2;
			case 3:	goto L3;
			case 4:	goto L4;
			case 5:	goto L5;
			case 6:	goto L6;
			case 7:	goto L7;
			case 8:	goto L8;
			case 9:	goto L9;
			default:	;
		}
	}

// ���� ����� LP();
L1:
//		ALFA=A1;
		for (i=0;i<25;i++)
			ALFA[i]=A1[i];
		NA=N;
		F1=TRUE;
		goto LIN;
L2:
//		PK=A1;
		for (i=0;i<25;i++)
			PK[i]=A1[i];
		NPK=N;
		F2=TRUE;
		goto LIN;
L3:
//		PA=A1;
		for (i=0;i<25;i++)
			PA[i]=A1[i];
		NPA=N;
		F3=TRUE;
		goto LIN;

L4:
//		GG=A1;
		for (i=0;i<25;i++)
			GG[i]=A1[i];
		NKG=N;						// ���������� ����������� � �������
		NK=N;
		F4=TRUE;
		goto LIN;
L6:
//		GO=A1;
		for (i=0;i<25;i++)
			GO[i]=A1[i];
		NKO=N;
		NK=N;
		F6=TRUE;
		goto LIN;
L8:
//		HAC=0;
		for (i=0;i<5;i++)	// Warning! size HAS[] is ?
			HAC[i]=0;

		for (i=0;i<N;i++)
			HAC[i]=A1[i];
		NHAC=N;
		FHAC=TRUE;
		goto LIN;
L9:
//		SHY=0;
		for (i=0;i<50;i++)
			SHY[i]=0;

		for (i=0;i<N;i++)
			SHY[i]=A1[i];
		NSHY=N;
		FSH=TRUE;
		goto LIN;
L5:
//		SGOR=AS;
		for (i1=0;i1<25;i1++)
			for (i2=0;i2<20;i2++)
				SGOR[i1][i2]=AS[i1][i2];
//		KGOR=AK;
		for (i1=0;i1<25;i1++)
			for (i2=0;i2<20;i2++)
				KGOR[i1][i2]=AK[i1][i2];
		F5=TRUE;
//		EG=ENT;
		for (i=0;i<25;i++)
			EG[i]=ENT[i];
//		NAMG=NAMA;
		for (i1=0;i1<25;i1++)
			for (i2=0;i2<7;i2++)
				NAMG[i1][i2]=NAMA[i1][i2];
		goto LIN;
L7:
//		SOK=AS;
		for (i1=0;i1<25;i1++)
			for (i2=0;i2<20;i2++)
				SOK[i1][i2]=AS[i1][i2];
//		KOK=AK;
		for (i1=0;i1<25;i1++)
			for (i2=0;i2<20;i2++)
				KOK[i1][i2]=AK[i1][i2];
		F7=TRUE;
//		EO=ENT;
		for (i=0;i<25;i++)
			EO[i]=ENT[i];
//		NAMO=NAMA;
		for (i1=0;i1<25;i1++)
			for (i2=0;i2<7;i2++)
				NAMO[i1][i2]=NAMA[i1][i2];
		goto LIN;
CBR:

	// ��������� ���������� � ��������
	for (I=1;I<=25;I++) {
		IO=IO+EO[I]*GO[I];
		IG=IG+EG[I]*GG[I];
	}

//********************************************
// �������� ��������� ����������� ��� ��������
//	M=0.0;
	for (i=0;i<100;i++)
		M[i]=0.0;
//	O=0.0;
	for (i=0;i<100;i++)
		O[i]=0.0;

	K2=0.0;
	// ������������ ����� �������������� ��������
	for (I1=1;I1<=25;I1++)
		for (I=1;I<=20;I++) {
			if (SOK[I1][I]==0)
				;
			else
				M[I1]=M[I1]+MU[SOK[I1][I]]*KOK[I1][I];
		}

	// �������� ���������� ������� ��������
	for (I1=1;I1<=25;I1++)
		for (I=1;I<=20;I++) {
			if ((M[I1]==0)||(SOK[I1][I]==0))
				;
			else
				O[AS[I1][I]]=O[SOK[I1][I]]+GO[I1]*KOK[I1][I]*1000/M[I1];
		}

//****************************************
// �������� ��������� ������� ��� ��������
//	M=0.0;
	for (i=0;i<100;i++)
		M[i]=0.0;
//	G=0.0;
	for (i=0;i<100;i++)
		G[i]=0.0;

	K1=0.0;
	// ������������ ����� �������������� ��������
	for (I1=1;I1<=25;I1++)
		for (I=1;I<=20;I++) {
			if (SGOR[I1][I]==0)
				;
			else
				M[I1]=M[I1]+MU[SGOR[I1][I]]*KGOR[I1][I];
		}

	// �������� ���������� ������� ��������
	for (I1=1;I1<=25;I1++)
		for (I=1;I<=20;I++) {
			if ((M[I1]==0)||(SGOR[I1][I]==0))
				;
			else
				G[SGOR[I1][I]]=G[SGOR[I1][I]]+GG[I1]*KGOR[I1][I]*1000/M[I1];
		}

//		K1=SUM(V*G);
//		K2=SUM(V*O);
		K1=0;
		K2=0;
		for (s=1;s<=99;s++) {
			K1+=V[s-1]*G[s];
			K2+=V[s-1]*O[s];
		}

		KAPPA=-K1/K2;
		goto MEND;

//**********************************************
// ������ ��� ���������� �������� � ������������
ERR:
	N=0;
	REJ="    ";
	if ((BEG[0]=='V')&&(BEG[1]=='R')) {
//		DISPLAY('ERROR');
		goto MV;
	}
	else {
//		PUT SKIP LIST('ERROR');
		goto LI;
	}

//AB:
	REJ="EOJ";
	goto LIN;


MEND:
	F8=(!F6)|(!F7);
	F4=(!F4)|(!F5);

	if ((!F)&&((NA==0)|F8)) {
//		PUT SKIP LIST ('������ ����������');
		goto LI;
	}
	return TRUE;
};

void logic_SORTV () {
// ������ ������ ������ �������� ���������� �������������� � ���������
/*
	A[ ]		  - ������ � �������������� ��� �����
	AK[ ]		- ������ � �������������� ��� �����������
	PRA		  - ���� ������������� ���������� ������� A[ ] � �������������� ��� �����
	PRAK	 - ���� ������������� ���������� ������� AK[ ] � �������������� ��� �����������
	RK			- ���������� ����������-�����������
*/
	//T3AM=1500;
	return;
};

void CALFN(const int N,const int NG,float &CK,float &T,
		   BOOL &CKACH,float &T2,float &T1,chem_dataGas cdG[],chem_dataCond cdC[]) {
	//DCL AT(5) DEC FLOAT(10);
	float	AT[5+1];
	CK = 0;
	//A[2500]
/*	double AK[]={0, 4,
		900, -3.9459573000, -117.7569900000, -1333.2721000000, 6590.5533000000, -2317.0757000000,
		2000, 3.2091870000, -1300.5239000000, 2912.1362000000, 1450.7057000000, -224.2944300000, 
		5000, 5.2124762000, -2879.2607000000, 5316.3926000000, 203.2142300000, -4.5503100000, 
		6000, 11.3649230000, 28215.2760000000, 7000.3953000000, -0.0717219390, 0.0043225331};*/
	double AK[]={0, 4,
		900,		-3.9459573000,		-117.7569900000, -1333.2721000000, 6590.5533000000, -2317.0757000000,
		2000,	3.2091870000,		-1300.5239000000, 2912.1362000000, 1450.7057000000, -224.2944300000, 
		5000,	5.2124762000,		-2879.2607000000, 5316.3926000000, 203.2142300000, -4.5503100000, 
		6000,	11.3649230000,		28215.2760000000, 7000.3953000000, -0.0717219390, 0.0043225331};

	double IS,SS,CPR;
	float RK = N-NG;
// ����������� ������������ �� ���������
	int	YI = 0;
	float R = 0;
	float W = 0;
	float C = 0;

// My code
	//T = 288;
	//const double fTrans = 4.1868;
	const float fTrans = 1;
	double tt = T/1000;
	double ResEnt,ResEntr,Entr,power,fff,ResCP;
	for (int i=0;i<NG;i++) {
		ResEnt = ResEntr = Entr = ResCP = 0;
		//
		ResEntr = cdG[i].factors[2]*log(tt);
		ResEnt = cdG[i].factors[1] + cdG[i].factors[2]*tt;//pow(tt,1);
		ResCP = cdG[i].factors[2];
		for (int i2=2;i2<NumFactorsGas;i2++) {
			power = pow(tt,i2-1);
			ResEnt += cdG[i].factors[i2+1]*power*tt;
			fff = i2;
			ResEntr += fff/(fff-1)*cdG[i].factors[i2+1]*power;
			ResCP += i2*cdG[i].factors[i2]*power;
		}
		Entr = (cdG[i].factors[0]+ResEntr*0.001);
		ResCP = 0.001*ResCP;

		ENT[i+1] = ResEnt;
		S[i+1] = Entr;
		CP[i+1] = ResCP;
	}
// My code end

	//�������� ������ ����������� � �������������� ��������� "cdG" �����
	R = T/1000;
	C = log(R)/1000;
	if (RK != 0)
	{
//L2:
		I = NG+1;
		Y = 1;
//L1:
		while (I<=N)
		{
			CP[I] = 0;
			S[I] = 0;
			ENT[I] = 1E9;
			X = floor(AK[Y]+0.5);
			for (I1=Y+1;I1<=Y+(X-1)*6+1;I1+=6) {
				//I1=Y+1 BY 6 TO Y+(X-1)*6+1;
//				if ((CKACH)&&(YI==0)) {//					if ((AK[I1]<T2)&&(AK[I1]>=T1)) {
				if ( (CKACH)&&(YI==0)&&(AK[I1]<T2)&&(AK[I1]>=T1) ) {
						T = AK[I1];
						YI = 1;
						
						R = T/1000;
						C = log(R)/1000;
						I = NG+1;
						Y = 1;
						X = floor(AK[Y]+0.5);
						I1=Y+1;
						//goto L2;
						continue;
				}
				if (AK[I1]>=T) {
					for (I2=1;I2<=5;I2++) {
						AT[6-I2] = AK[I1+I2];
					}
					IS = 0;
					SS = 0;
					CPR = 0;
					//goto L;
//L:
					for (J=1;J<=2;J++) {	// DO J=1,2
						W = AT[J];
						IS = (IS+W)*R;
						fff = J;
						SS = (SS+(4-fff)/(3-fff)*W)*R;
						CPR = (CPR+(4-J)*W)*R;
					}
					W = AT[3];
					ENT[I] = (IS+W)*R+AT[4];
					S[I] = SS/1000+W*C+AT[5];
					CP[I] = (CPR+W)/1000;

					if ((CKACH)&&(AK[I1]==T)) {
						IC = I;
						CK = ENT[I]-AK[I1+10]-((AK[I1+7]*R+AK[I1+8])*R+AK[I1+9])*R;
					}
					break;
				}
			}
//L10:
			I = I+1;
			Y = Y+6*X+1;
			//if (I<=N)
			//	goto L1;
		}
	}
//L0:
	CKACH = FALSE;
	X = 0;
/*
	for (I=1;I<=NG;I++) {
		X = X+1;
		IS = 0; 
		SS = 0; 
		CPR = 0;
		for (J=1;J<=6;J++) {
			W = A[(X-1)*9+J];
			IS = (IS+W)*R;
			SS = (SS+(8-J)/(7-J)*W)*R;
			CPR = (CPR+(8-J)*W)*R;
		}
		W = A[(X-1)*9+7];
		ENT[I] = (IS+W)*R+A[(X-1)*9+8];
//		S[I] = SS/1000+N?G+A[(X-1)*9+9]; // ���������� ������
		S[I] = SS/1000+W*C+A[(X-1)*9+9];
		CP[I] = (CPR+W)/1000;
	}
*/
	if (FSH) {
		for (I=1;I<=N;I++) {
			if (I==SHY[DR]) {
				DR = DR+1;
				if (FRY) {
					ENT[I] = 1E9;
					S[I] = 0;
				}
			}
			else {
				if (!FRY) {
					ENT[I] = 1E9;
					S[I] = 0;
				}
			}
		}
	}
	return;
}

void Init_CALBR_STREAM(void) {
	nCALBR_STREAM = 0;
	nCALBR_STREAM_size = strlen(cCALBR_STREAM);
	return;
};

void _GET_CALBR(const char *pS, int nLetters) {
/*
	int nC=0,nL=0;
	while (nC<nLetters) {
		c = cCALBR_STREAM[nCALBR_STREAM+nC];
		if ((((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z'))) )
			nL++;
		nC++;
	}
*/
	strncpy(const_cast<char *>(pS),&cCALBR_STREAM[nCALBR_STREAM],nLetters);
	nCALBR_STREAM+=nLetters;

	while (nCALBR_STREAM<nCALBR_STREAM_size)
	{
		c = cCALBR_STREAM[nCALBR_STREAM];
		if (((c>='0')&&(c<='9'))||
			(((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z')))||
			(c=='-')||(c=='+')
			)
			break;
		else 
			nCALBR_STREAM++;
	}
	return;
};

void _GET_CALBR(int *pI) {
	int i=atoi(&cCALBR_STREAM[nCALBR_STREAM]);
	*pI=i;

	while (nCALBR_STREAM<nCALBR_STREAM_size)
	{
		c = cCALBR_STREAM[nCALBR_STREAM];
		if (((c>='0')&&(c<='9'))||(c=='.')||(c=='-')||(c=='+'))
			nCALBR_STREAM++;
		else
			break;
	}

	while (nCALBR_STREAM<nCALBR_STREAM_size)
	{
		c = cCALBR_STREAM[nCALBR_STREAM];
		if (((c>='0')&&(c<='9'))||
			(((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z')))||
			(c=='-')||(c=='+')
			)
			break;
		else 
			nCALBR_STREAM++;
	}

	return;
};

void _GET_CALBR(float *pI) {
	float f=atof(&cCALBR_STREAM[nCALBR_STREAM]);
	*pI=f;

	while (nCALBR_STREAM<nCALBR_STREAM_size)
	{
		c = cCALBR_STREAM[nCALBR_STREAM];
		if (((c>='0')&&(c<='9'))||(c=='.')||(c=='-')||(c=='+'))
			nCALBR_STREAM++;
		else
			break;
	}

	while (nCALBR_STREAM<nCALBR_STREAM_size)
	{
		c = cCALBR_STREAM[nCALBR_STREAM];
		if (((c>='0')&&(c<='9'))||
			(((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z')))||
			(c=='-')||(c=='+')
			)
			break;
		else 
			nCALBR_STREAM++;
	}
	return;
};

void empty_IPAGE() {
};

BOOL PRINT_TestBTMIN(float &MS,BOOL &KAMEPA,BOOL &FIS,float &PYD,float &PYDP,
	double &IS,float &T,float &PP) 
{
	if ((KAMEPA)||(FIS))
		return FALSE;
	if (!FKP)
		return FALSE;

	if (MS!=0)
		RR = 847.9/MS;

	float _W = sqrt(HK-IS);
	PYD = 9.328*_W;
	_W = 91.48*_W;
	BT = RR*T/_W;
	RR = (RR+RC)/2;			// RC ��� RR � ������, � �� ��������� ����� ��� RR �� ����� � �����
	PYDP = PYD+BT;
	BT = BT * PPK/PP;			// BT = ((847.9/MS)*T/(91.48*sqrt(HK-IS)))*PPK/PP
	if (BTMIN>BT)	{
		BTMIN = BT; 
		_ST[1] = PP; _ST[2] = IS; _ST[3] = T; _ST[4] = MS; _ST[5] = CP3;
		_ST[8] = PYD; _ST[9] = PYDP; _ST[10] = BT; _ST[11] = ZK;
		return FALSE;
	}
	else {
		return TRUE; 
	}
};

void PRINT_CameraCombustion(BOOL &KAMEPA,BOOL &FIS,float &PP,double &IS,float &T,
		float &MS,type_ParamInCC &_ParamInCC) 
{
	if ((!KAMEPA)||(FIS))
		return;

	if (MS!=0)
		RR = 847.9/MS;
	_ParamInCC.Alfa			= ALFA[IA];
	_ParamInCC.K_actually	= ALFA[IA]*KAPPA;
	_ParamInCC.P_KAM			= PP*1.03323;
	_ParamInCC.I				= IS;
	_ParamInCC.T				= T + 0.5;
	_ParamInCC.MS				= MS;
	_ParamInCC.CP3				= CP3;
	_ParamInCC.Z				= ZK;
	return;
	//...	SA[20];
/* 1	�����,	K,			P,		T,
	5	I,			S,			� ��,	CPR,
	9	CP3,		R,			K3,	Z,
	13	I ��,		I ���,	B,		F,
	17	F/F��,	NU,		LD,	DTP	*/
	return;
};

void PRINT_CriticalSection(float &MS,BOOL &KAMEPA,BOOL &FIS,double &W,float &PYD,float &PYDP,
	float &E,double &IS,float &T,float &PP,type_CriticalSection &_ParamInCriticalSection)
{
	if ((KAMEPA)||(FIS))
		return;
	if (!FKP)
		return;

	if (MS!=0)
		RR = 847.9/MS;

	W = sqrt(HK-IS);
	PYD = 9.328*W;
	W = 91.48*W;
	BT = RR*T/W;
	RR = (RR+RC)/2;			// RC ��� RR � ������, � �� ��������� ����� ��� RR �� ����� � �����
	PYDP = PYD+BT;
	E = (HK-IS)/(TK-T);
	E = E/(E-0.023485*RR);
	BT = BT * PPK/PP;			// BT = ((847.9/MS)*T/(91.48*sqrt(HK-IS)))+PPK/PP
	PP = _ST[1]; IS = _ST[2]; T = _ST[3]; MS = _ST[4]; CP3 = _ST[5];
	PYD = _ST[8]; PYDP = _ST[9]; BT = _ST[10]; ZK = _ST[11];

	FKP = FALSE;
	PKP = PP + 0.01*PPK;

	// ZKK ��� ZK � ������
	// ZK ���� ����������, ZK == 1 ����� � PABHPAC(); ��������� ������
	if (ZKK!=1)
		RR = log(PPK/PP) / log(PPK*T*MGK*(1-ZK)/(PP*TK*MG*(1-ZKK)));

	_ParamInCriticalSection.Alfa			= ALFA[IA];
	_ParamInCriticalSection.K_actually	= ALFA[IA]*KAPPA;
	_ParamInCriticalSection.PKP			= PP*1.03323;
	_ParamInCriticalSection.I				= IS;
	_ParamInCriticalSection.T				= T + 0.5;
	_ParamInCriticalSection.MS				= MS;
	_ParamInCriticalSection.CP3			= CP3;
	_ParamInCriticalSection.K3				= RR;
	_ParamInCriticalSection.B				= BTMIN;			// ��������� ��������
	_ParamInCriticalSection.PYD			= PYD;
	_ParamInCriticalSection.PYDP			= PYDP;
	_ParamInCriticalSection.F				= BT/PPK;		// �������� ������� �������
	_ParamInCriticalSection.F_div_FKP	= BT/BTMIN;
	_ParamInCriticalSection.Z				= ZK;
	return;
}

void PRINT_Section(float &MS,BOOL &KAMEPA,BOOL &FIS,double &W,float &PYD,float &PYDP,
	float &E,double &IS,float &T,float &PP,type_Section &_ParamInSection)
{
	if ((KAMEPA)||(FKP)||(FIS)||(PKP==0)||(!FF)||(PX!=PY))
		return;

	if (MS!=0)
		RR = 847.9/MS;

	W = sqrt(HK-IS);
	PYD = 9.328*W;
	W = 91.48*W;
	BT = RR*T/W;
	RR = (RR+RC)/2;			// RC ��� RR � ������, � �� ��������� ����� ��� RR �� ����� � �����
	PYDP = PYD+BT;
	E = (HK-IS)/(TK-T);
	E = E/(E-0.023485*RR);
	BT = BT * PPK/PP;			// BT = ((847.9/MS)*T/(91.48*sqrt(HK-IS)))+PPK/PP
	// ZKK ��� ZK � ������
	// ZK ���� ����������, ZK == 1 ����� � PABHPAC(); ��������� ������
	if (ZKK!=1)
		RR = log(PPK/PP) / log(PPK*T*MGK*(1-ZK)/(PP*TK*MG*(1-ZKK)));

	_ParamInSection.Alfa			= ALFA[IA];
	_ParamInSection.K_actually	= ALFA[IA]*KAPPA;
	_ParamInSection.P_Section	= PP*1.03323;
	_ParamInSection.I				= IS;
	_ParamInSection.T				= T + 0.5;
	_ParamInSection.MS			= MS;
	_ParamInSection.CP3			= CP3;
	_ParamInSection.K3			= RR;
	_ParamInSection.B				= BTMIN;			// ��������� ��������
	_ParamInSection.PYD			= PYD;
	_ParamInSection.PYDP			= PYDP;
	_ParamInSection.F				= BT/PPK;		// �������� ������� �������
	_ParamInSection.F_div_FKP	= BT/BTMIN;		// ����. ������� ���������� �����
	_ParamInSection.Z				= ZK;
	return;
}

void MENJE_TransitionToFindCriticalSection(BOOL &KAMEPA,BOOL &F3C,
		float &H,double &IS,float &T,double &SS,float &T2,float &T1,float &PP) {
// ������� �� �� ���
	if (T<T3AM)
		F3C=TRUE;
	KAMEPA=FALSE;
	TK=T;
	HK=IS;
	RC=RR;
	H=SS;
	PPA=0.61*PP;//0.61: �� ������� ������ �������� � ������� ������ ��� � �� ��� ��� � ��
	// 0.6^0.12 = 0,9405		// ��� ������ ������� 0.6: 0.6 =	0.12/(1.2-1)
	T = T*pow(0.6,0.12);		// T=T*0.6**0.12; ����������� �������� **
	T2=TK;
	MGK=MG;
	ZKK=ZK;
	BTMIN=1E6;
	FKP=TRUE;
	//PP = 0.61*PP-0.01*PPK
	MENJE_NextStepToFindCriticalSection(T1,PP);//LKP://PPA=PPA-0.01*PPK; //T1=0; //PP=PPA;
};

void MENJE_NextStepToFindCriticalSection(float &T1,float &PP) {
	PPA=PPA-0.01*PPK;
	T1=0;
	PP=PPA;	
};

void MENJE_CameraCombustion(float MC[],int &M,int &JE,BOOL &F0T,BOOL &FPP,BOOL &CKACH,
	BOOL &FIS,BOOL &KAMEPA,BOOL &F3C,BOOL &FP3,float &H,float &T,float &T2,float &T1,float &PP) 
{
	if ((KAMEPA)||(FIS)) return;

	F0T=CKACH=FALSE;

	if (IPK==0) {
		if (!_NewAlfa(MC, M, JE, F0T, FPP, FIS, H, T))
			return;
	}

	if (!KAMEPA) {
//L3:
		IPK=IPK+1;
		while (IPK>NPK) {
			IPK=0;
			if (!_NewAlfa(MC, M, JE, F0T, FPP, FIS, H, T))
				return;
			IPK=IPK+1;
		};

		KAMEPA=TRUE;
		PPK=PK[IPK]/1.03323;
		PP=PPK;
		F3C=FP3=FALSE;
		PX=PY=FX=FY=PKP=0;

		if (IPK==1) {
			T1 = 0;
			T2 = 3000;
			T = (float)1084.8;
		}
		else {
			T2=TK;
			T=TK;
			T1=0;
			H=HK;
		}
		JE=2;
		return;
	}
};

/*void FirstCamera() {
	KAMEPA=TRUE;
	PPK=PK[IPK]/1.03323;
	PP=PPK;
	F3C=FP3=FALSE;
	PX=PY=FX=FY=PKP=0;
	T1 = 0;
	T2 = 3000;
	T = (float)1084.8;
}
void ContinueCamera() {
	KAMEPA=TRUE;
	PPK=PK[IPK]/1.03323;
	PP=PPK;
	F3C=FP3=FALSE;
	PX=PY=FX=FY=PKP=0;
	T2=TK;
	T=TK;
	T1=0;
	H=HK;
}
*/
void MENJE_CameraCombustion_IS(float MC[],int &M,int &JE,BOOL &F0T,BOOL &FPP,BOOL &CKACH,
		BOOL &FIS,BOOL &KAMEPA,BOOL &FT,BOOL &F3C,
		BOOL &FV,BOOL &FP3,float &H,float &P1,
		float &T,float &T2,float &T1,float &PP,float &P2) 
{
	F0T=CKACH=FALSE;

L2:
	if (!_NewAlfa(MC, M, JE, F0T, FPP, FIS, H, T))
		return;

//L3:
	IPK=IPK+1;
	while (IPK>NPK) {
		IPK=0;
		if (!FIS)
			if (!_NewAlfa(MC, M, JE, F0T, FPP, FIS, H, T))
				return;
		IPK=IPK+1;
	};

	KAMEPA=TRUE;
	PPK=PK[IPK]/1.03323;
	PP=PPK;
	F3C=FP3=FALSE;
	PX=PY=FX=FY=PKP=0;

	if (!FIS) {
		if (IPK==1) {
			T1 = 0;
			T2 = 3000;
			T = (float)1084.8;
		}
		else {
			T2=TK;
			T=TK;
			T1=0;
			H=HK;
		}
		JE=2;
		return;
	}
	else {
		// ���� ��� ����� FIS!=0 �� ������ �� �� ������ TRUE
		if (IPK!=1) {		//if (FIS)
			JE=3;
			return;
		} 
		else {				//if (IPK==1) {
			// ��������� ����� 'L1:'
			if ( (!FF)||(PY==PX) ) {
				if (IPA<=NPA) {	// ���� FIS == TRUE
					IPA=IPA+1;
					JE=2;
					T=PA[IPA];
					return;
				} 
				else {				// ���� FIS == TRUE
					IPA=0;
					goto L2;
				}
			} 
			else {	// if ((FF)&&(PY!=PX))// ��������� ����� 'L1:'
				// ��������� ��������� ������� �����
//				_Func_if_FF__PX_PY_FX_FY_PPA(PP);
//				_Func__T2_PPA_PP_P1_P2_T_FP3_T1(FT,F3C,FP3,T3AM,P1,T,T2,T1,PP,P2);
				return;
			}
		}
	}
};

void MENJE_TransitionToFindSection(BOOL &F0T,BOOL &CKACH,BOOL &FIS,BOOL &KAMEPA,BOOL &FT,
		BOOL &F3C,BOOL &FP3,float &P1,float &T,float &T2,float &T1,float &PP,float &P2) 
{
	F0T=CKACH=FALSE;
	if ((FIS)||(KAMEPA)||(FKP)||(PKP==0)||(PY!=PX))
		return;

	IPA=IPA+1;
	if (IPA>NPA)
		return;

	//_Func_if_FF (PP);// ��������� ��������� �������� ������ �����
	FA=PA[IPA];
	PX=(float)0.1E-18;
	PY=0;
	FX=0;
	FY=0;
	PPA=PPK*0.06/(FA-1);
	DA=1;
	if ((NPA!=IPA)&&(FA>=PA[IPA+1]))
		DA=-1;

//LL:	_Func__T2_PPA_PP_P1_P2_T_FP3_T1(FT,F3C,FP3,T3AM,P1,T,T2,T1,PP,P2);
	if (PPA<PP)
		T2=T;
	else
		T2=TK;
	T2=T2+5;
	if ((FF)&&((DA*PPA)>(DA*PKP)))
		PPA=PKP;
	if (!FT) {
		if (PPA>PPK)
			PPA=PPK;
	}
	T = T*pow((PPA/PP),0.15);	// T=T*(PPA/PP)**0.15;	����������� �������� **
	T1=0;
	PP=PPA;
	return;
};

BOOL MENJE_NextStepToFindSection(float &MS,double &W,float &E,double &IS,
		BOOL &F0T,BOOL &CKACH,BOOL &FIS,BOOL &KAMEPA,BOOL &FT,
		BOOL &F3C,BOOL &FP3,float &P1,float &T,float &T2,float &T1,float &PP,float &P2) 
{
	if ((KAMEPA)||(FKP)||(FIS)||(PKP==0)||(!FF)||(PX==PY))
		return -1;

	if (MS!=0)
		RR = 847.9/MS;

	W = sqrt(HK-IS);
	W = 91.48*W;
	BT = RR*T/W;
	BT = BT * PPK/PP;			// BT = ((847.9/MS)*T/(91.48*sqrt(HK-IS)))+PPK/PP
	E = RR;
	RR = FA-BT/BTMIN;	// BT/BTMIN - ����. ������� ���������� �����
	RR = RR*DA;			// DA - �����������
	if ( (fabs(RR) > (0.001*FA)) && ((fabs(PY-PX)) >= (0.01*PY))) {
		// MENJE
		F0T=CKACH=FALSE;
//LR:	_Func_if_FF__PX_PY_FX_FY_PPA(PP);// ��������� ��������� ������� �����
		if (RR>0)
			{ PY=PP; FY=RR; }	// ���� ������� ������� ������ �������
		else
			{ PX=PP; FX=RR; }	// ���� ������� ������� ��������� �������
		if ((FX*FY)==0) {	
			// ���� ����� ���� ���� ������� ������� 
			// �� �������� ������� � �� ������ ������ �������
			if (RR<0)
				PPA=PP*2;		// ���� ������� ������� ��������� �������, �������� ��������
			else
				PPA=PP/2;
		} 
		else
			PPA=PX+FX*(PY-PX)/(FX-FY);	// � ������ ������ ���� ������:PX -_ FX*(PY-PX)/(FX-FY)

		if (PPA<=0)
			return -2;

//LL:	_Func__T2_PPA_PP_P1_P2_T_FP3_T1(FT,F3C,FP3,T3AM,P1,T,T2,T1,PP,P2);
		if (PPA<PP)
			T2=T;
		else
			T2=TK;
		T2=T2+5;
		if ((FF)&&((DA*PPA)>(DA*PKP)))
			PPA=PKP;
		if (!FT) {
			if (PPA>PPK)
				PPA=PPK;
		}
		T = T*pow((PPA/PP),0.15);	// T=T*(PPA/PP)**0.15;	����������� �������� **
		T1=0;
		PP=PPA;
		return FALSE;	// FALSE - ���������� ������ ��� ����� ����������� � ��������
	}
	PX = PY;
	RR = E;
	return TRUE;
};
