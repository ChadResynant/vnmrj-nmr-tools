//typedef struct {
//    char a[20*MAXSTR]; // A string with the arrayed parameter names
//    int  b[20];        // Number of characters in each name
//    int  c;            // Number of parameters
//    int  d;            // Number of indicies
//    int  e[20];        // arraysize for each parameter (not displayed)
//    int  f[20];        // column index for each parameter
//    int  g[20];        // array index for each column using "ix"
//    int  i[20];        // hasarray bit for each column
//    int  j[20];        // arraysize for each column
//} AR;

void print_ary(int *ary, int len) {
    int i;
    printf("{");
    for(i=0;i<len;i++) {
        printf("%d", ary[i]);
        if(i<len-1) {
            printf(", ");
        }
    }
    printf("}\n");
}

void printAR(AR a) {
    printf("AR:\n");
    printf("\ta = %s", a.a);
    printf("\tb = ");
    print_ary(a.b,20);
    printf("\tc = %d\n", a.c);
    printf("\td = %d\n", a.d);
    printf("\te = ");
    print_ary(a.e,20);
    printf("\tf = ");
    print_ary(a.f,20);
    printf("\tg = "); 
    print_ary(a.g,20);
    printf("\ti = ");
    print_ary(a.i,20);
    printf("\tj = ");
    print_ary(a.j,20);
}


//typedef struct {
//   char   seqName[NSUFFIX];      //parameter group name
//   char   pattern[NPATTERN];     //shapefile name
//   char   sh[2];                 //shape: "c", "l", "t"
//   char   ch[NCH];               //output channel
//   char   pol[3];                //polarity (n, ud, uu)
//   double t;                     //duration of the sequence
//   double a;                     //rf amplitude
//   double d;                     //delta parameter of tangent shape -d to +d
//   double b;                     //beta parameter of tangent shape
//   double phInt;                 //internal phase accumulation bookkeeping
//   double phAccum;               //phase accumulated due to offset
//   int    nRec;                  //copy number
//   int    preset1;               //preset the wavefrom generator (INOVA)
//   int    preset2;               //separately clear the wavefrom generator (INOVA)
//   double strtdelay;             //wavegen start delay (INOVA)
//   double offstdelay;            //wavegen offset delay (INOVA)
//   double apdelay;               //ap bus and wfg stop delay (INOVA)
//   int    n90;                   //clock ticks consumed in min time interval
//   int    n;                     //clock ticks consumed by the pulse
//   double of;                    //frequency
//   int    calc;
//   int    hasArray;
//   AR     array;
//} RAMP;

void printRAMP(RAMP r) {
    printf("RAMP:\n");
    printf("\tseqName = %s\n", r.seqName);
    printf("\tpattern = %s\n", r.pattern);
    printf("\tsh = %s\n", r.sh);
    printf("\tch = %s\n", r.ch);
    printf("\tpol = %s\n", r.pol);
    printf("\tt = %f\n", r.t);
    printf("\ta = %f\n", r.a);
    printf("\td = %f\n", r.d);
    printf("\tb = %f\n", r.b);
    printf("\tphInt = %f\n", r.phInt);
    printf("\tphAccum = %f\n", r.phAccum);
    printf("\tnRec = %d\n", r.nRec);
    printf("\tpreset1 = %d\n", r.preset1);
    printf("\tpreset2 = %d\n", r.preset2);
    printf("\tstrtdelay = %f\n", r.strtdelay);
    printf("\toffstdelay = %f\n", r.offstdelay);
    printf("\tapdelay = %f\n", r.apdelay);
    printf("\tn90 = %d\n", r.n90);
    printf("\tn = %d\n", r.n);
    printf("\tof = %f\n", r.of);
    printf("\tcalc = %d\n", r.calc);
    printf("\thasArray = %d\n", r.hasArray);
    printAR(r.array);
}

void initAR(AR *ar) {
    ar->c = 0;
    ar->d = 0;
    int i;
    for(i=0;i<20;i++) {
        ar->b[i] = 0;
        ar->e[i] = 0;
        ar->f[i] = 0;
        ar->g[i] = 0;
        ar->i[i] = 0;
        ar->j[i] = 0;
    }
}

typedef struct {
    char   seqName[NSUFFIX];  //parameter group name
    char   ch[NCH];
    RAMP   in;
    RAMP   mix;
    RAMP   out;
} R2TCKL;

char *getnamej(char* var_name, char* seq_name) {
    int n = strlen(seq_name);
    int m = strlen(var_name);
    char *out_str = (char*)malloc((n+m+1)*sizeof(char));
    strcpy(out_str, var_name);
    strcat(out_str, seq_name);
    out_str[n+m] = '\0';  // TO BE SAFE
    return out_str;
}

R2TCKL getr2tckl(char *seqName, int iph, double p, double phint, int iRec, int calc) {
    R2TCKL r = {};

    initAR(&(r.in.array));
    initAR(&(r.mix.array));
    initAR(&(r.out.array));

    char *varname;
    char lpattern[NPATTERN];
    int lix;

    sprintf(r.seqName,"%s",seqName);
    varname = getnamej("ch",r.seqName);
    Getstr(varname,r.ch,sizeof(r.ch));


    sprintf(r.in.seqName,"%s_in",r.seqName);
    sprintf(r.mix.seqName,"%s_mix",r.seqName);
    sprintf(r.out.seqName,"%s_out",r.seqName);

    r.in.calc = calc;
    r.in.array = parsearry(r.in.array);
    r.in.phInt = phint;
    r.in.phAccum = p;
    r.in.nRec = iRec;
    r.in.preset1 = 0;
    r.in.preset2 = 0;
    r.in.strtdelay = 0;
    r.in.offstdelay = 0;
    r.in.apdelay = 0;
    varname = getnamej("b",r.in.seqName);
    r.in.b = getval(varname);
    r.in.array = disarry(varname, r.in.array);
    strcpy(r.in.ch,r.ch);
    strcpy(r.in.sh,"t");
    varname = getnamej("t",r.in.seqName);
    r.in.t = getval(varname);
    r.in.array = disarry(varname, r.in.array);
    strcpy(r.in.pol,"n");
    r.in.n90 = (PWRF_DELAY > 0.0 ? 40 : 40); // CMR 11/17/23 change from 16:8 to 40:40

    r.mix.calc = calc;
    r.mix.phInt = phint;
    r.mix.phAccum = p;
    r.mix.nRec = iRec;
    r.mix.array = parsearry(r.mix.array);
    r.mix.preset1 = 0;
    r.mix.preset2 = 0;
    r.mix.strtdelay = 0;
    r.mix.offstdelay = 0;
    r.mix.apdelay = 0;
    
    varname = getnamej("b",r.mix.seqName);
    r.mix.b = getval(varname);
    r.mix.array = disarry(varname, r.mix.array);

    strcpy(r.mix.ch,r.ch);
    strcpy(r.mix.sh,"t");
    varname = getnamej("t",r.mix.seqName);
    r.mix.t = getval(varname);
    r.mix.array = disarry(varname, r.mix.array);
    r.mix.n90 = (PWRF_DELAY > 0.0 ? 40 : 40); // CMR 11/17/23 change from 16:8 to 40:40

    r.out.calc = calc;
    r.out.array = parsearry(r.out.array);
    r.out.phInt = phint;
    r.out.phAccum = p;
    r.out.nRec = iRec;
    r.out.preset1 = 0;
    r.out.preset2 = 0;
    r.out.strtdelay = 0;
    r.out.offstdelay = 0;
    r.out.apdelay = 0;
    varname = getnamej("b",r.out.seqName);
    r.out.b = getval(varname);
    r.out.array = disarry(varname, r.out.array);
    strcpy(r.out.ch,r.ch);
    strcpy(r.out.sh,"t");
    varname = getnamej("t",r.out.seqName);
    r.out.t = getval(varname);
    r.out.array = disarry(varname, r.out.array);
    strcpy(r.out.pol,"ud");
    r.out.n90 = (PWRF_DELAY > 0.0 ? 40 : 40); // CMR 11/17/23 change from 16:8 to 40:40



    varname = getnamej("a",r.seqName);
    r.mix.a = getval(varname);
    varname = getnamej("d",r.seqName);
    r.mix.d = getval(varname);
    if(r.mix.d > 0) { strcpy(r.mix.pol,"n"); }
    else { strcpy(r.mix.pol,"ud"); }

    r.in.d = (r.mix.a - r.mix.d)/2;
    r.out.d = -(r.mix.a + r.mix.d)/2;

    r.in.a = r.mix.a - (r.mix.d + r.in.d);
    r.out.a = r.mix.a + (r.mix.d + r.out.d);

    varname = getnamej("a",r.seqName);
    r.in.array = disarry(varname, r.in.array);
    r.mix.array = disarry(varname, r.mix.array);
    r.out.array = disarry(varname, r.out.array);

    varname = getnamej("d",r.seqName);
    r.mix.array = disarry(varname, r.in.array);
    r.mix.array = disarry(varname, r.mix.array);
    r.mix.array = disarry(varname, r.out.array);



    sprintf(lpattern,"%s%d",r.in.seqName,r.in.nRec);
    r.in.hasArray = hasarry(r.in.array, lpattern);
    lix = arryindex(r.in.array);
    if (r.in.calc > 0) {
        sprintf(r.in.pattern,"%s%d_%d",r.in.seqName,r.in.nRec,lix);
        if (r.in.hasArray == 1) {
            r.in = make_ramp(r.in);
        }
    }

    sprintf(lpattern,"%s%d",r.mix.seqName,r.mix.nRec);
    r.mix.hasArray = hasarry(r.mix.array, lpattern);
    lix = arryindex(r.mix.array);
    if (r.mix.calc > 0) {
        sprintf(r.mix.pattern,"%s%d_%d",r.mix.seqName,r.mix.nRec,lix);
        if (r.mix.hasArray == 1) {
            r.mix = make_ramp(r.mix);
        }
    }

    sprintf(lpattern,"%s%d",r.out.seqName,r.out.nRec);
    r.out.hasArray = hasarry(r.out.array, lpattern);
    lix = arryindex(r.out.array);
    if (r.out.calc > 0) {
        sprintf(r.out.pattern,"%s%d_%d",r.out.seqName,r.out.nRec,lix);
        if (r.out.hasArray == 1) {
            r.out = make_ramp(r.out);
        }
    }

    printf("\nr2t_ramp_in\n");
    dump_ramp_array(r.in);
    printf("\nr2t_ramp_mix\n");
    dump_ramp_array(r.mix);
    printf("\nr2t_ramp_out\n");
    dump_ramp_array(r.out);


    return r;
}


void _r2tckl(R2TCKL r2t, double phIn, double phMix, double phOut) {
    if(r2t.in.t>0) {
        _ramp(r2t.in, phIn);
    }
    _ramp(r2t.mix, phMix);
    if(r2t.out.t>0) {
        _ramp(r2t.out, phOut);
    }
}
