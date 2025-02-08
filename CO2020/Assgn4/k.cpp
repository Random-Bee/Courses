void get_coeffs(int nx, int ny, double *x, double *y, double *dx, double *dy, double **aP, double **aE, double **aW, double **aN, double **aS, double **b, double **T, double **dTdx, double **dTdy, double **kdiff, double **bcleft, double **bcrght, double **bctop, double **bcbot)
{

    int i, j;
    double pj, qj, rj, pi, qi, ri, k;
    double hxhy_sq, dtdx, dtdy;

    // calculate dTdx
    calc_dTdx(nx, ny, T, dTdx, dx);

    // calculate dTdy
    calc_dTdy(nx, ny, T, dTdy, dy);

    // calculate diffusivity at [x, y]; may be dependent on T
    calc_diffusivity(nx, ny, x, y, T, kdiff);

    // calculate sources Su, Sp at [x, y]; may be dependent on T
    calc_sources(nx, ny, x, y, dx, dy, T, b);

    // populate values in BC arrays
    set_boundary_conditions(nx, ny, x, y, dx, dy, T, bcleft, bcrght, bctop, bcbot);
    //   for (i=0;i<nx;i++) {
    //     printf("Bottom pi[%d] = %lf\n",i, bcbot[i][0]);
    //   }

    // start populating the coefficients
    hxhy_sq = dx[0] * dx[0] / (dy[0] * dy[0]);

    // ------ Step 1 :: interior points ------
    for (i = 1; i < nx - 1; i++)
        for (j = 1; j < ny - 1; j++)
        {
            k = kdiff[i][j];
            dtdx = dTdx[i][j];
            dtdy = dTdy[i][j];
            aP[i][j] = 2.0*k*(1.0+hxhy_sq);
            aE[i][j] = -1.0*(k+B*dx[0]*dtdx/2.0);
            aW[i][j] = -1.0*(k-B*dx[0]*dtdx/2.0);
            aN[i][j] = -1.0*(k*hxhy_sq+B*hxhy_sq*dy[0]*dtdy/2.0);
            aS[i][j] = -1.0*(k*hxhy_sq-B*hxhy_sq*dy[0]*dtdy/2.0);
        }

    // ------ Step 2 :: left boundary ----------
    i = 0;
    for (j = 0; j < ny; j++)
    {
        k = kdiff[i][j];
        dtdx = dTdx[i][j];
        dtdy = dTdy[i][j];
        pj = bcleft[j][0];
        qj = bcleft[j][1];
        rj = bcleft[j][2];

        aP[i][j] = 2.0*k*(1.0+hxhy_sq)*pj - qj*2.0*dx[0]*(k-B*dx[0]*dtdx/2.0);
        aE[i][j] = -2.0 *k* pj;
        aW[i][j] = 0.0;
        aN[i][j] = -1.0*(k*hxhy_sq+B*hxhy_sq*dy[0]*dtdy/2.0)*pj;
        aS[i][j] = -1.0*(k*hxhy_sq-B*hxhy_sq*dy[0]*dtdy/2.0)*pj;
        b[i][j] = b[i][j] * pj - 2.0 * dx[0] * rj*(k-B*dx[0]*dtdx/2.0);
    }
    // ------ Step 2 :: left boundary done ---

    // ------ Step 3 :: right boundary ----------
    i = nx - 1;
    for (j = 0; j < ny; j++)
    {
        k = kdiff[i][j];
        dtdx = dTdx[i][j];
        dtdy = dTdy[i][j];
        pj = bcrght[j][0];
        qj = bcrght[j][1];
        rj = bcrght[j][2];

        aP[i][j] = 2.0*k*(1.0+hxhy_sq)*pj + qj*2.0*dx[0]*(k+B*dx[0]*dtdx/2.0);
        aE[i][j] = 0.0;
        aW[i][j] = -2.0 *k* pj;
        aN[i][j] = -1.0*(k*hxhy_sq+B*hxhy_sq*dy[0]*dtdy/2.0)*pj;
        aS[i][j] = -1.0*(k*hxhy_sq-B*hxhy_sq*dy[0]*dtdy/2.0)*pj;
        b[i][j] = b[i][j] * pj + 2.0 * dx[0] * rj*(k+B*dx[0]*dtdx/2.0);
    }
    // ------ Step 3 :: right boundary done ---

    // ------ Step 4 :: bottom boundary ----------
    j = 0;
    for (i = 1; i < nx - 1; i++)
    {
        k = kdiff[i][j];
        dtdx = dTdx[i][j];
        dtdy = dTdy[i][j];
        pi = bcbot[i][0];
        qi = bcbot[i][1];
        ri = bcbot[i][2];
        aP[i][j] = 2.0 *k* (1.0 + hxhy_sq) * pi - 2.0 *dy[0] * qi*(k*hxhy_sq-B*hxhy_sq*dy[0]*dtdy/2.0);
        aE[i][j] = -1.0*(k+B*dx[0]*dtdx/2.0)*pi;
        aW[i][j] = -1.0*(k-B*dx[0]*dtdx/2.0)*pi;
        aN[i][j] = -2.0*k*hxhy_sq*pi;
        aS[i][j] = 0.0;
        b[i][j] = b[i][j] * pi - 2.0 *dy[0] * ri*(k*hxhy_sq-B*hxhy_sq*dy[0]*dtdy/2.0);
    }
    // ------ Step 4 :: bottom boundary done ---

    // ------ Step 5 :: top boundary ----------
    j = ny - 1;
    for (i = 1; i < nx - 1; i++)
    {
        k = kdiff[i][j];
        dtdx = dTdx[i][j];
        dtdy = dTdy[i][j];
        pi = bctop[i][0];
        qi = bctop[i][1];
        ri = bctop[i][2];

        aP[i][j] = 2.0 *k* (1.0 + hxhy_sq) * pi + 2.0 *dy[0] * qi*(k*hxhy_sq+B*hxhy_sq*dy[0]*dtdy/2.0);
        aE[i][j] = -1.0*(k+B*dx[0]*dtdx/2.0)*pi;
        aW[i][j] = -1.0*(k-B*dx[0]*dtdx/2.0)*pi;
        aN[i][j] = 0.0;
        aS[i][j] = -2.0*k*hxhy_sq*pi;
        b[i][j] = b[i][j] * pi + 2.0 *dy[0] * ri*(k*hxhy_sq+B*hxhy_sq*dy[0]*dtdy/2.0);
    }
    // ------ Step 5 :: top boundary done ---

    // debug
    //   for(i=0; i<nx; i++)
    //    for(j=0; j<ny; j++)
    //    {
    //      printf("%d %d %lf %lf %lf %lf %lf %lf\n", i, j, aP[i][j], aE[i][j], aW[i][j], aN[i][j], aS[i][j], b[i][j]);
    //    }
}