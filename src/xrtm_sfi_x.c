static void SFI_LAYER(int i_layer, double ptau, int i_four,
                      int n_quad, int n_stokes, int n_derivs, int n_umus,
                      double qf, double *qx_v, double *qw_v, double F_0, double *umus,
                      double omega, double *omega_l, double ltau, double *ltau_l,
                      double btran, double *btran_l,
                      double as_0, double *as_0_l, double atran, double *atran_l,
                      double *P_u0_pm, double **P_uq_pp, double **P_uq_mp,
                      TYPE *nu, TYPE **X_p, TYPE **X_m,
                      double *F_p, double *F_m,
                      double **At_p, double **At_m,
                      double *F0_p, double *F0_m, double *F1_p, double *F1_m,
                      double **P_u0_pm_l, double ***P_uq_pp_l, double ***P_uq_mp_l,
                      TYPE **nu_l, TYPE ***X_p_l, TYPE ***X_m_l,
                      double **F_p_l, double **F_m_l,
                      double ***At_p_l, double ***At_m_l,
                      TYPE *B, TYPE **B_l,
                      double *I_0, double **I_0_l, int offset, double *I_u, double **I_u_l,
                      int add_single_scattering, int greens, int solar, int thermal, int utau_output,
                      uchar *derivs_layers, uchar *derivs_beam, uchar *derivs_thermal,
                      work_data work, int flag, double c[][2], double c_l[][128][2], double *P_q0_mm, double *P_q0_pm) {

     int ii;
     int j;
     int k;
     int l;

     int n_quad_v;
     int n_umus_v;

     double a1;
     double a1_l;
     double a2;
double a2_l;
double a3;
double a3_l;
     double solfac;

     double *q1;
     double *q2;
     double *q3;

     double *u1;
     double *u2;
     double *u3;
     double *u4;
double *u5;
double **u21;
double **u22;
double **u24;
     double *A;

     double *e1;
     double **e1_l;
     double e2;
     double e2_l;

     double *ptau_l;
#ifndef REAL
     double **qq1;

     double **uq1;
     double **uq2;
#endif
     double *Fu_0;
     double *Du_0;
     double *Gu_0;
     double  Eu_0;

     TYPE x1;
     TYPE x1_l;

     TYPE *e3;
     TYPE *e4;
     TYPE *e5;

     TYPE *e3_l;
     TYPE *e4_l;
     TYPE *e5_l;

     TYPE **xqq1;
     TYPE **xqq2;
     TYPE **xqq3;
     TYPE **xqq4;
/*
     TYPE **xuq1;
     TYPE **xuq2;
*/
     TYPE **Xu_p;
     TYPE **Xu_m;
     TYPE **Yu_p;
     TYPE **Yu_m;

     TYPE **Du_p;
     TYPE **Du_m;
     TYPE   Eu_p;
     TYPE   Eu_m;


     n_quad_v = n_quad * n_stokes;

     n_umus_v = n_umus  * n_stokes;


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     q1   = get_work1(&work, WORK_DX);
     q2   = get_work1(&work, WORK_DX);
     q3   = get_work1(&work, WORK_DX);

     u1   = get_work1(&work, WORK_DU);
     u2   = get_work1(&work, WORK_DU);
     u3   = get_work1(&work, WORK_DU);
     u4   = get_work1(&work, WORK_DU);
u5  = get_work1(&work, WORK_DU);
u21 = alloc_array2_d(2, n_umus_v);
u22 = alloc_array2_d(2, n_umus_v);
u24 = alloc_array2_d(2, n_umus_v);
     A    = get_work1(&work, WORK_DX);

     e1   = get_work1(&work, WORK_DU);

     Fu_0 = get_work1(&work, WORK_DU);
     Du_0 = get_work1(&work, WORK_DU);

     e3   = get_work1(&work, WORK_XX);
     e4   = get_work1(&work, WORK_XX);
     e5   = get_work1(&work, WORK_XX);
#ifndef REAL
     qq1  = get_work1(&work, WORK_DXX);

     uq1  = get_work1(&work, WORK_DUX);
     uq2  = get_work1(&work, WORK_DUX);
#endif
     xqq1 = get_work1(&work, WORK_XXX);
     xqq2 = get_work1(&work, WORK_XXX);
     xqq3 = get_work1(&work, WORK_XXX);
     xqq4 = get_work1(&work, WORK_XXX);
/*
     xuq1 = get_work1(&work, WORK_XUX);
     xuq2 = get_work1(&work, WORK_XUX);
*/
     Xu_p = get_work1(&work, WORK_XUX);
     Xu_m = get_work1(&work, WORK_XUX);

     Du_p = get_work1(&work, WORK_XUX);
     Du_m = get_work1(&work, WORK_XUX);

     if (n_derivs > 0 && flags_or(derivs_layers, n_derivs)) {
          ptau_l = get_work1(&work, WORK_DDERIVS);

          e1_l   = get_work2(&work, WORK_DU, WORK_DERIVS_V, NULL);

          e3_l   = get_work1(&work, WORK_XX);
          e4_l   = get_work1(&work, WORK_XX);
          e5_l   = get_work1(&work, WORK_XX);

          Yu_p   = get_work1(&work, WORK_XUX);
          Yu_m   = get_work1(&work, WORK_XUX);
     }

     if (solar && n_derivs > 0 && flags_or(derivs_beam, n_derivs))
          Gu_0   = get_work1(&work, WORK_DU);


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     solfac = F_0 / (4. * PI);


     a1 = (1. + (i_four == 0 ? 1. : 0.)) / 4.;

     dvec_scale(a1, qw_v, A, n_quad_v);


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     for (j = 0; j < n_umus_v; ++j) {
          if (! flag)
               e1[j] = exp(-(ltau - ptau) / umus[j]);
          else
               e1[j] = exp(-        ptau  / umus[j]);
     }

     for (j = 0; j < n_derivs; ++j) {
          if (derivs_layers[j]) {
               ptau_l[j] = ptau * ltau_l[j] / ltau;

               for (k = 0; k < n_umus_v; ++k) {
                    if (! flag)
                         e1_l[j][k] = -(ltau_l[j] - ptau_l[j]) / umus[k] * e1[k];
                    else
                         e1_l[j][k] = -             ptau_l[j]  / umus[k] * e1[k];
               }
          }
     }


     for (j = 0; j < n_derivs; ++j) {
          for (k = 0; k < n_umus_v; ++k) {
               I_u_l[j][offset + k] = I_0_l[j][k] * e1[k];

               if (derivs_layers[j])
                    I_u_l[j][offset + k] += I_0[k] * e1_l[j][k];
          }
     }

     for (j = 0; j < n_umus_v; ++j)
          I_u[offset + j] = I_0[j] * e1[j];


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
if (solar) {

if (! greens) {
     dm_v_diag_mul(A, F_p, q1, n_quad_v);
     dm_v_mul(P_uq_pp, q1, n_umus_v, n_quad_v, u1);
     dm_v_diag_mul(A, F_m, q2, n_quad_v);
     dm_v_mul(P_uq_mp, q2, n_umus_v, n_quad_v, u2);
     dvec_add(u1, u2, u1, n_umus_v);

     a1 = btran * solfac;

     for (j = 0; j < n_umus_v; ++j) {
if (add_single_scattering)
          u1[j] += a1 * P_u0_pm[j];
          Fu_0[j] = omega * u1[j];
     }

     e2 = exp(-ptau * as_0);

     for (j = 0; j < n_umus_v; ++j) {
          if (! flag)
               Du_0[j] = (e2 - atran * e1[j]) / (1. + umus[j] * as_0);
          else
               Du_0[j] = (e2 -         e1[j]) / (1. - umus[j] * as_0);
     }

     for (j = 0; j < n_derivs; ++j) {
          if (derivs_beam[j])
               dvec_zero(Gu_0, n_umus_v);

          if (derivs_layers[j]) {
               dm_v_mul(P_uq_pp_l[j], q1, n_umus_v, n_quad_v, u2);
               dm_v_mul(P_uq_mp_l[j], q2, n_umus_v, n_quad_v, u3);
               dvec_add(u2, u3, u2, n_umus_v);

               for (k = 0; k < n_umus_v; ++k)
if (! add_single_scattering)
                    Gu_0[k] += omega_l[j] * u1[k] + omega * (                       u2[k]);
else
                    Gu_0[k] += omega_l[j] * u1[k] + omega * (a1 * P_u0_pm_l[j][k] + u2[k]);
          }

          if (derivs_beam[j]) {
               dm_v_diag_mul(A, F_p_l[j], q3, n_quad_v);
               dm_v_mul(P_uq_pp, q3, n_umus_v, n_quad_v, u3);

               dm_v_diag_mul(A, F_m_l[j], q3, n_quad_v);
               dm_v_mul(P_uq_mp, q3, n_umus_v, n_quad_v, u4);

               dvec_add(u3, u4, u3, n_umus_v);
if (  add_single_scattering)
               a2 = btran_l[j] * solfac;

               for (k = 0; k < n_umus_v; ++k)
if (! add_single_scattering)
                    Gu_0[k] +=                      omega * (                       u3[k]);
else
                    Gu_0[k] +=                      omega * (a2 * P_u0_pm     [k] + u3[k]);
          }

          if (derivs_layers[j])
               e2_l = (-ptau_l[j] * as_0 - ptau * as_0_l[j]) * e2;

          for (k = 0; k < n_umus_v; ++k) {
               if (derivs_beam[j])
                    I_u_l[j][offset + k] += Gu_0[k] * Du_0[k];

               if (derivs_layers[j]) {
                    if (! flag)
                         Eu_0 = ((e2_l - atran_l[j] * e1[k] - atran * e1_l[j][k]) - Du_0[k] * ( umus[k] * as_0_l[j])) / (1. + umus[k] * as_0);
                    else
                         Eu_0 = ((e2_l -                              e1_l[j][k]) - Du_0[k] * (-umus[k] * as_0_l[j])) / (1. - umus[k] * as_0);

                    I_u_l[j][offset + k] += Fu_0[k] * Eu_0;
               }
          }
     }

     for (j = 0; j < n_umus_v; ++j)
          I_u[offset + j] += Fu_0[j] * Du_0[j];
}
else {
#ifdef REAL
     double x2;
     double e6;

     double H_p;
     double H_m;

     double g;

     double *a_q;
     double *b_q;
     double *c_q;
     double *e_q;
     double *f_q;

     double *N;

     double *D_p;
     double *D_m;

     a_q = get_work1(&work, WORK_DX);
     b_q = get_work1(&work, WORK_DX);
     c_q = get_work1(&work, WORK_DX);
     e_q = get_work1(&work, WORK_DX);
     f_q = get_work1(&work, WORK_DX);

     N   = get_work1(&work, WORK_DX);

     D_p = get_work1(&work, WORK_DX);
     D_m = get_work1(&work, WORK_DX);

     for (j = 0; j < n_quad_v; ++j)
          c_q[j] = qx_v[j] * qw_v[j];

     e2 = exp(-as_0 * ptau);

     for (j = 0; j < n_quad_v; ++j) {
          e3[j] = XEXP(-nu[j] *  ptau);
          e4[j] = XEXP(-nu[j] *  ltau);
          e5[j] = XEXP(-nu[j] * (ltau - ptau));
     }
for (j = 0; j < n_umus_v; ++j) {
     x2 = umus[j] * as_0;
     e6 = exp(-(ltau - ptau) / umus[j]);

     for (k = 0; k < n_quad_v; ++k) {
          x1 = umus[j] * nu[k];

          if (! flag) {
               H_p    = (e3[k] - e4[k] * e1[j]) / (1. + x1);
               H_m    = (e5[k] -         e1[j]) / (1. - x1);

               g      = (e2 - atran * e6   ) / (1. + x2);

               D_p[k] = btran * (-atran * H_m + g) / (as_0 + nu[k]);
               D_m[k] = btran * (         H_p - g) / (as_0 - nu[k]);
          }
          else {
               H_p    = (e3[k] -         e1[j]) / (1. - x1);
               H_m    = (e5[k] - e4[k] * e1[j]) / (1. + x1);

               g      = (e2 -         e1[j]) / (1. - x2);

               D_p[k] = btran * (-atran * H_m + g) / (as_0 + nu[k]);
               D_m[k] = btran * (         H_p - g) / (as_0 - nu[k]);
          }
     }

     for (k = 0; k < n_quad_v; ++k) {
          N[k] = 0.;
          for (l = 0; l < n_quad_v; ++l)
               N[k] += c_q[l] * (X_p[l][k] * X_p[l][k] - X_m[l][k] * X_m[l][k]);
     }

     for (k = 0; k < n_quad_v; ++k) {
          e_q[k] = 0.;
          f_q[k] = 0.;
          for (l = 0; l < n_quad_v; ++l) {
               e_q[k] += qw_v[l] * (P_q0_pm[l] * -X_p[l][k] + P_q0_mm[l] *  X_m[l][k]);
               f_q[k] += qw_v[l] * (P_q0_mm[l] *  X_p[l][k] + P_q0_pm[l] * -X_m[l][k]);
          }
     }

     for (k = 0; k < n_quad_v; ++k) {
          a_q[k] = omega * e_q[k] / N[k];
          b_q[k] = omega * f_q[k] / N[k];
     }

     dmat_diag_mul(A, X_p, xqq1, n_quad_v, n_quad_v);
     dmat_diag_mul(A, X_m, xqq2, n_quad_v, n_quad_v);

     dmat_gxgxmx(0, P_uq_pp, 0, xqq1,  1., Xu_p, 0., n_umus_v, n_quad_v, n_quad_v);
     dmat_gxgxmx(0, P_uq_mp, 0, xqq2, -1., Xu_p, 1., n_umus_v, n_quad_v, n_quad_v);

     dmat_gxgxmx(0, P_uq_pp, 0, xqq2,  1., Xu_m, 0., n_umus_v, n_quad_v, n_quad_v);
     dmat_gxgxmx(0, P_uq_mp, 0, xqq1, -1., Xu_m, 1., n_umus_v, n_quad_v, n_quad_v);
if (add_single_scattering)
     I_u[offset + j] += solfac * btran * omega * P_u0_pm[j] * g;

     for (k = 0; k < n_quad_v; ++k)
          I_u[offset + j] += solfac * omega * (a_q[k] * Xu_p[j][k] * D_m[k] + b_q[k] * Xu_m[j][k] * D_p[k]);
}
#endif
}
}

     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
if (thermal && i_four == 0) {
     double tau0;
     double tau0_l;
     double umus_sign;

     if (! flag) {
          tau0      =  ltau;
          umus_sign =  1.;
     }
     else {
          tau0      =  0.;
          umus_sign = -1.;
     }

     dvec_zero(u1, n_umus_v);

     for (j = 0; j < 2; ++j) {
          dm_v_diag_mul(qw_v, At_p[j], q1, n_quad_v);
          dm_v_mul(P_uq_pp, q1, n_umus_v, n_quad_v, u2);
          dm_v_diag_mul(qw_v, At_m[j], q1, n_quad_v);
          dm_v_mul(P_uq_mp, q1, n_umus_v, n_quad_v, u3);
          dvec_add(u2, u3, u21[j], n_umus_v);

          dvec_scale(.5 * omega, u21[j], u22[j], n_umus_v);

          a1 = (1. - omega) * c[i_layer][j];

          a2 = pow(ptau, j);
          a3 = pow(tau0, j);

          for (k = 0; k < n_umus_v; ++k) {
               u22[j][k] += a1;

               if (j == 0)
                    u24[j][k] = 1. - e1[k];
               else
                    u24[j][k] = a2 - a3 * e1[k] + j * umus_sign * umus[k] * u24[j-1][k];

               u1[k] += u22[j][k] * u24[j][k];
          }
     }

     for (j = 0; j < n_derivs; ++j) {
          if (! flag)
               tau0_l = ltau_l[j];
          else
               tau0_l = 0.;

          dvec_zero(u5, n_umus_v);

          for (k = 0; k < 2; ++k) {
               dvec_zero(u2, n_umus_v);
if (derivs_layers[j]) {
               dm_v_diag_mul(qw_v, At_p[k], q1, n_quad_v);
               dm_v_mul(P_uq_pp_l[j], q1, n_umus_v, n_quad_v, u3);
               dvec_add(u2, u3, u2, n_umus_v);
}
if (derivs_thermal[j]) {
               dm_v_diag_mul(qw_v, At_p_l[j][k], q1, n_quad_v);
               dm_v_mul(P_uq_pp, q1, n_umus_v, n_quad_v, u3);
               dvec_add(u2, u3, u2, n_umus_v);
}
if (derivs_layers[j]) {
               dm_v_diag_mul(qw_v, At_m[k], q1, n_quad_v);
               dm_v_mul(P_uq_mp_l[j], q1, n_umus_v, n_quad_v, u3);
               dvec_add(u2, u3, u2, n_umus_v);
}
if (derivs_thermal[j]) {
               dm_v_diag_mul(qw_v, At_m_l[j][k], q1, n_quad_v);
               dm_v_mul(P_uq_mp, q1, n_umus_v, n_quad_v, u3);
               dvec_add(u2, u3, u2, n_umus_v);
}
               dvec_scale(.5 * omega_l[j], u21[k], u3, n_umus_v);
               dvec_scale(.5 * omega,      u2,     u2, n_umus_v);
               dvec_add(u2, u3, u2, n_umus_v);

               a1_l = -omega_l[j] * c[i_layer][k] + (1. - omega) * c_l[i_layer][j][k];
if (k > 0) {
     if (! derivs_layers[j])
               a2_l = 0.;
     else
               a2_l = ptau_l[j] * k * pow(ptau, k - 1);
               a3_l = tau0_l    * k * pow(tau0, k - 1);
}
               for (l = 0; l < n_umus_v; ++l) {
                    u2[l] += a1_l;

                    u5[l] += u2[l] * u24[k][l];
if (derivs_layers[j]) {
                    if (k == 0)
                         u4[l] = -e1_l[j][l];
                    else
                         u4[l] = a2_l - a3_l * e1[l] - a3 * e1_l[j][l] + k * umus_sign * umus[l] * u4[l];

                    u5[l] += u22[k][l] * u4[l];
}
               }
          }

          for (k = 0; k < n_umus_v; ++k)
               I_u_l[j][offset + k] += u5[k];
     }

     for (j = 0; j < n_umus_v; ++j)
          I_u[offset + j] += u1[j];
}

     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     ii = i_layer * n_quad_v * 2;
#ifdef REAL
     dmat_diag_mul(A, X_p, xqq1, n_quad_v, n_quad_v);
     dmat_diag_mul(A, X_m, xqq2, n_quad_v, n_quad_v);
/*
     dmat_mul(P_uq_pp, xqq1, n_umus_v, n_quad_v, n_quad_v, xuq1);
     dmat_mul(P_uq_mp, xqq2, n_umus_v, n_quad_v, n_quad_v, xuq2);
     xmat_sub(xuq1, xuq2, Xu_p, n_umus_v, n_quad_v);
*/
     dmat_gxgxmx(0, P_uq_pp, 0, xqq1,  1., Xu_p, 0., n_umus_v, n_quad_v, n_quad_v);
     dmat_gxgxmx(0, P_uq_mp, 0, xqq2, -1., Xu_p, 1., n_umus_v, n_quad_v, n_quad_v);
/*
     dmat_mul(P_uq_pp, xqq2, n_umus_v, n_quad_v, n_quad_v, xuq1);
     dmat_mul(P_uq_mp, xqq1, n_umus_v, n_quad_v, n_quad_v, xuq2);
     xmat_sub(xuq1, xuq2, Xu_m, n_umus_v, n_quad_v);
*/
     dmat_gxgxmx(0, P_uq_pp, 0, xqq2,  1., Xu_m, 0., n_umus_v, n_quad_v, n_quad_v);
     dmat_gxgxmx(0, P_uq_mp, 0, xqq1, -1., Xu_m, 1., n_umus_v, n_quad_v, n_quad_v);
#else
     dzmat_diag_mul(A, X_p, xqq1, n_quad_v, n_quad_v);
     dzmat_diag_mul(A, X_m, xqq2, n_quad_v, n_quad_v);
/*
     dzmat_mul2(P_uq_pp, xqq1, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
     dzmat_mul2(P_uq_mp, xqq2, n_umus_v, n_quad_v, n_quad_v, xuq2, qq1, uq1, uq2);
     xmat_sub(xuq1, xuq2, Xu_p, n_umus_v, n_quad_v);
*/
     dzmat_gxgxmx(0, P_uq_pp, 0, xqq1,  1., Xu_p, 0., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
     dzmat_gxgxmx(0, P_uq_mp, 0, xqq2, -1., Xu_p, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
/*
     dzmat_mul2(P_uq_pp, xqq2, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
     dzmat_mul2(P_uq_mp, xqq1, n_umus_v, n_quad_v, n_quad_v, xuq2, qq1, uq1, uq2);
     xmat_sub(xuq1, xuq2, Xu_m, n_umus_v, n_quad_v);
*/
     dzmat_gxgxmx(0, P_uq_pp, 0, xqq2,  1., Xu_m, 0., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
     dzmat_gxgxmx(0, P_uq_mp, 0, xqq1, -1., Xu_m, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
#endif
     for (j = 0; j < n_quad_v; ++j) {
          e3[j] = XEXP(-nu[j] *  ptau);
          e4[j] = XEXP(-nu[j] *  ltau);
          e5[j] = XEXP(-nu[j] * (ltau - ptau));
     }

     for (j = 0; j < n_umus_v; ++j) {
          for (k = 0; k < n_quad_v; ++k) {
               x1 = umus[j] * nu[k];

               if (! flag) {
                    Du_p[j][k] = (e3[k] - e4[k] * e1[j]) / (1. + x1);
                    Du_m[j][k] = (e5[k] -         e1[j]) / (1. - x1);
               }
               else {
                    Du_p[j][k] = (e3[k] -         e1[j]) / (1. - x1);
                    Du_m[j][k] = (e5[k] - e4[k] * e1[j]) / (1. + x1);
               }
          }
     }

     for (j = 0; j < n_derivs; ++j) {
          if (derivs_layers[j]) {
#ifdef REAL
               dmat_diag_mul(A, X_p_l[j], xqq3, n_quad_v, n_quad_v);
               dmat_diag_mul(A, X_m_l[j], xqq4, n_quad_v, n_quad_v);
/*
               dmat_mul(P_uq_pp_l[j], xqq1, n_umus_v, n_quad_v, n_quad_v, xuq1);
               dmat_mul(P_uq_mp_l[j], xqq2, n_umus_v, n_quad_v, n_quad_v, xuq2);
               xmat_sub(xuq1, xuq2, Yu_p, n_umus_v, n_quad_v);
*/
               dmat_gxgxmx(0, P_uq_pp_l[j], 0, xqq1,  1., Yu_p, 0., n_umus_v, n_quad_v, n_quad_v);
               dmat_gxgxmx(0, P_uq_mp_l[j], 0, xqq2, -1., Yu_p, 1., n_umus_v, n_quad_v, n_quad_v);
/*
               dmat_mul(P_uq_pp, xqq3, n_umus_v, n_quad_v, n_quad_v, xuq1);
               xmat_add(Yu_p, xuq1, Yu_p, n_umus_v, n_quad_v);
               dmat_mul(P_uq_mp, xqq4, n_umus_v, n_quad_v, n_quad_v, xuq1);
               xmat_sub(Yu_p, xuq1, Yu_p, n_umus_v, n_quad_v);
*/
               dmat_gxgxmx(0, P_uq_pp, 0, xqq3,  1., Yu_p, 1., n_umus_v, n_quad_v, n_quad_v);
               dmat_gxgxmx(0, P_uq_mp, 0, xqq4, -1., Yu_p, 1., n_umus_v, n_quad_v, n_quad_v);
/*
               dmat_mul(P_uq_pp_l[j], xqq2, n_umus_v, n_quad_v, n_quad_v, xuq1);
               dmat_mul(P_uq_mp_l[j], xqq1, n_umus_v, n_quad_v, n_quad_v, xuq2);
               xmat_sub(xuq1, xuq2, Yu_m, n_umus_v, n_quad_v);
*/
               dmat_gxgxmx(0, P_uq_pp_l[j], 0, xqq2,  1., Yu_m, 0., n_umus_v, n_quad_v, n_quad_v);
               dmat_gxgxmx(0, P_uq_mp_l[j], 0, xqq1, -1., Yu_m, 1., n_umus_v, n_quad_v, n_quad_v);
/*
               dmat_mul(P_uq_pp, xqq4, n_umus_v, n_quad_v, n_quad_v, xuq1);
               xmat_add(Yu_m, xuq1, Yu_m, n_umus_v, n_quad_v);
               dmat_mul(P_uq_mp, xqq3, n_umus_v, n_quad_v, n_quad_v, xuq1);
               xmat_sub(Yu_m, xuq1, Yu_m, n_umus_v, n_quad_v);
*/
               dmat_gxgxmx(0, P_uq_pp, 0, xqq4,  1., Yu_m, 1., n_umus_v, n_quad_v, n_quad_v);
               dmat_gxgxmx(0, P_uq_mp, 0, xqq3, -1., Yu_m, 1., n_umus_v, n_quad_v, n_quad_v);
#else
               dzmat_diag_mul(A, X_p_l[j], xqq3, n_quad_v, n_quad_v);
               dzmat_diag_mul(A, X_m_l[j], xqq4, n_quad_v, n_quad_v);
/*
               dzmat_mul2(P_uq_pp_l[j], xqq1, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
               dzmat_mul2(P_uq_mp_l[j], xqq2, n_umus_v, n_quad_v, n_quad_v, xuq2, qq1, uq1, uq2);
               xmat_sub(xuq1, xuq2, Yu_p, n_umus_v, n_quad_v);
*/
               dzmat_gxgxmx(0, P_uq_pp_l[j], 0, xqq1,  1., Yu_p, 0., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
               dzmat_gxgxmx(0, P_uq_mp_l[j], 0, xqq2, -1., Yu_p, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
/*
               dzmat_mul2(P_uq_pp, xqq3, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
               xmat_add(Yu_p, xuq1, Yu_p, n_umus_v, n_quad_v);
               dzmat_mul2(P_uq_mp, xqq4, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
               xmat_sub(Yu_p, xuq1, Yu_p, n_umus_v, n_quad_v);
*/
               dzmat_gxgxmx(0, P_uq_pp, 0, xqq3,  1., Yu_p, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
               dzmat_gxgxmx(0, P_uq_mp, 0, xqq4, -1., Yu_p, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
/*
               dzmat_mul2(P_uq_pp_l[j], xqq2, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
               dzmat_mul2(P_uq_mp_l[j], xqq1, n_umus_v, n_quad_v, n_quad_v, xuq2, qq1, uq1, uq2);
               xmat_sub(xuq1, xuq2, Yu_m, n_umus_v, n_quad_v);
*/
               dzmat_gxgxmx(0, P_uq_pp_l[j], 0, xqq2,  1., Yu_m, 0., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
               dzmat_gxgxmx(0, P_uq_mp_l[j], 0, xqq1, -1., Yu_m, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
/*
               dzmat_mul2(P_uq_pp, xqq4, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
               xmat_add(Yu_m, xuq1, Yu_m, n_umus_v, n_quad_v);
               dzmat_mul2(P_uq_mp, xqq3, n_umus_v, n_quad_v, n_quad_v, xuq1, qq1, uq1, uq2);
               xmat_sub(Yu_m, xuq1, Yu_m, n_umus_v, n_quad_v);
*/
               dzmat_gxgxmx(0, P_uq_pp, 0, xqq4,  1., Yu_m, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
               dzmat_gxgxmx(0, P_uq_mp, 0, xqq3, -1., Yu_m, 1., n_umus_v, n_quad_v, n_quad_v, qq1, uq1, uq2);
#endif
               for (k = 0; k < n_quad_v; ++k) {
                    e3_l[k] = (-nu_l[j][k] * ptau  - nu[k] * ptau_l[j])  * e3[k];
                    e4_l[k] = (-nu_l[j][k] * ltau - nu[k] * ltau_l[j]) * e4[k];
                    e5_l[k] = (-nu_l[j][k] * (ltau - ptau) - nu[k] * (ltau_l[j] - ptau_l[j])) * e5[k];
               }
          }

          for (k = 0; k < n_umus_v; ++k) {
               for (l = 0; l < n_quad_v; ++l) {
                    I_u_l[j][offset + k] += omega_l[j] * XREAL(B     [ii + l] * Xu_p[k][l] * Du_p[k][l] + B     [ii + l + n_quad_v] * Xu_m[k][l] * Du_m[k][l]);
                    I_u_l[j][offset + k] += omega      * XREAL(B_l[j][ii + l] * Xu_p[k][l] * Du_p[k][l] + B_l[j][ii + l + n_quad_v] * Xu_m[k][l] * Du_m[k][l]);

                    if (derivs_layers[j]) {
if (LEGACY_MODE) {
                         if (! flag) {
                              Eu_p = (((-nu_l[j][l] *         ptau  - nu[l] *              ptau_l[j])  * e3[l] - ((-nu_l[j][l] * ltau - nu[l] * ltau_l[j]) * e4[l] * e1[k] + e4[l] * e1_l[j][k])) + Du_p[k][l] * -umus[k] * nu_l[j][l]) / (1. + umus[k] * nu[l]);
                              Eu_m = (((-nu_l[j][l] * (ltau - ptau) - nu[l] * (ltau_l[j] - ptau_l[j])) * e5[l] -                                                                     e1_l[j][k])  + Du_m[k][l] *  umus[k] * nu_l[j][l]) / (1. - umus[k] * nu[l]);
                         }
                         else {
                              Eu_p = (((-nu_l[j][l] *         ptau  - nu[l] *              ptau_l[j])  * e3[l] -                                                                     e1_l[j][k])  + Du_p[k][l] *  umus[k] * nu_l[j][l]) / (1. - umus[k] * nu[l]);
                              Eu_m = (((-nu_l[j][l] * (ltau - ptau) - nu[l] * (ltau_l[j] - ptau_l[j])) * e5[l] - ((-nu_l[j][l] * ltau - nu[l] * ltau_l[j]) * e4[l] * e1[k] + e4[l] * e1_l[j][k])) + Du_m[k][l] * -umus[k] * nu_l[j][l]) / (1. + umus[k] * nu[l]);
                         }
}
else {
                         x1   = umus[k] * nu[l];

                         x1_l = umus[k] * nu_l[j][l];

                         if (! flag) {
                              Eu_p = ((e3_l[l] - (e4_l[l] * e1[k] + e4[l] * e1_l[j][k])) + Du_p[k][l] * -x1_l) / (1. + x1);
                              Eu_m = ((e5_l[l] -                            e1_l[j][k])  + Du_m[k][l] *  x1_l) / (1. - x1);
                         }
                         else {
                              Eu_p = ((e3_l[l] -                            e1_l[j][k])  + Du_p[k][l] *  x1_l) / (1. - x1);
                              Eu_m = ((e5_l[l] - (e4_l[l] * e1[k] + e4[l] * e1_l[j][k])) + Du_m[k][l] * -x1_l) / (1. + x1);
                         }
}
                         I_u_l[j][offset + k] += omega * XREAL(B[ii + l           ] * Yu_p[k][l] * Du_p[k][l] + B[ii + l           ] * Xu_p[k][l] * Eu_p +
                                                               B[ii + l + n_quad_v] * Yu_m[k][l] * Du_m[k][l] + B[ii + l + n_quad_v] * Xu_m[k][l] * Eu_m);
                    }
               }
          }
     }

     for (j = 0; j < n_umus_v; ++j) {
          for (k = 0; k < n_quad_v; ++k) {
               I_u[offset + j] += omega * XREAL(B[ii + k] * Xu_p[j][k] * Du_p[j][k] + B[ii + k + n_quad_v] * Xu_m[j][k] * Du_m[j][k]);
          }
     }


     free_array2_d(u21);
     free_array2_d(u22);
     free_array2_d(u24);
}



/*******************************************************************************
 *
 ******************************************************************************/
static void SFI(int i_four,
                int n_quad, int n_stokes, int n_derivs, int n_layers, int n_umus,
                double qf, double *qx_v, double *qw_v, double F_0,
                int n_ulevels, int *ulevels, double *utaus, double *umus,
                double *omega, double **omega_l, double *ltau, double **ltau_l,
                double *btran, double **btran_l,
                double *as_0, double **as_0_l, double *atran, double **atran_l,
                double **P_u0_pm, double ***P_uq_pp, double ***P_uq_mp,
                TYPE **nu, TYPE ***X_p, TYPE ***X_m,
                double **F_p, double **F_m,
                double ***At_p, double ***At_m,
                double **F0_p, double **F0_m, double **F1_p, double **F1_m,
                double ***P_u0_pm_l, double ****P_uq_pp_l, double ****P_uq_mp_l,
                TYPE ***nu_l, TYPE ****X_p_l, TYPE ****X_m_l,
                double ***F_p_l, double ***F_m_l,
                double ****At_p_l, double ****At_m_l,
                TYPE *B, TYPE **B_l,
                double *I_0, double **I_0_l, double **I_u, int offset, double ***I_u_l,
                int add_single_scattering, int greens, int solar, int thermal, int utau_output,
                derivs_data *derivs, work_data work, int flag, double c[][2], double c_l[][128][2], double **P_q0_mm, double **P_q0_pm) {

     int i;
     int i1;
     int i2;
     int j;
     int k;

     int n_umus_v;

     int inc;

     int i_out_level;

     double ptau;

     double **I_u_l2;


     n_umus_v = n_umus  * n_stokes;


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     if (! flag) {
          i1          = n_layers - 1;
          i2          = ulevels[0] - 1;
          inc         = -1;
          i_out_level = n_ulevels - 1;
     }
     else {
          i1          = 0;
          if (! utau_output)
               i2 = ulevels[n_ulevels - 1];
          else
               i2 = ulevels[n_ulevels - 1] + 1;
          inc         =  1;
          i_out_level = 0;
     }


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
/*
     if (n_derivs > 0) {
          for (i = 0; i < n_ulevels; ++i) {
               for (j = 0; j < n_derivs; ++j) {
                    init_array1_d(I_u_l[i][j] + offset, n_umus_v, 0.);
               }
          }
     }
*/

     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     if ((! flag && ! utau_output && i1 + 1 == ulevels[i_out_level]) || (flag && ! utau_output && i1 == ulevels[i_out_level]) || (! flag && utau_output && i1 + 1 == ulevels[i_out_level] && utaus[i_out_level] == 0.) || (flag && utau_output && i1 == ulevels[i_out_level] && utaus[i_out_level] == 0.)) {
          for (i = 0; i < n_umus_v; ++i) {
               I_u[i_out_level][offset + i] = I_0[i];
          }
          for (i = 0; i < n_derivs; ++i) {
               for (j = 0; j < n_umus_v; ++j) {
                    I_u_l[i_out_level][i][offset + j] = I_0_l[i][j];
               }
          }

          i_out_level += inc;
     }


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
if (! greens) {

     for (i = i1; i != i2; i += inc) {

          if ((! flag && utau_output && i == ulevels[i_out_level]) || (flag && utau_output && i == ulevels[i_out_level])) {
               while (i_out_level >= 0 && i_out_level < n_ulevels && i == ulevels[i_out_level]) {
                    ptau = utaus[i_out_level];

                    if (n_derivs > 0)
                         I_u_l2 = I_u_l[i_out_level];
if (n_derivs == 0) {
     if (! solar) {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, NULL, NULL);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, NULL, NULL);
     }
     else {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, NULL, NULL);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, NULL, NULL);
     }
}
else {

if (! flags_or2(derivs->layers, n_layers, n_derivs)) {
     if (! solar) {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            NULL,               work, flag, c, c_l, NULL, NULL);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
     }
     else {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, NULL, NULL);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
     }
}
else {
     if (! solar) {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            NULL,               work, flag, c, c_l, NULL, NULL);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
     }
     else {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, NULL, NULL);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
     }
}

}
                    i_out_level += inc;
               }
          }

          if (! flag)
               ptau = 0.;
          else
               ptau = ltau[i];
if (n_derivs == 0) {
     if (! solar) {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, NULL, NULL);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, NULL, NULL);
     }
     else {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, NULL, NULL);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, NULL, NULL);
     }
}
else {

if (! flags_or2(derivs->layers, n_layers, n_derivs)) {
     if (! solar) {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,           NULL,                work, flag, c, c_l, NULL, NULL);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
     }
     else {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, NULL, NULL);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
    }
}

else {
     if (! solar) {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,           NULL,               work, flag, c, c_l, NULL, NULL);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,           derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
     }
     else {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, NULL, NULL);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, NULL, NULL);
     }
}

}
          if ((! flag && ! utau_output && i == ulevels[i_out_level]) || (flag && ! utau_output && i + 1 == ulevels[i_out_level])) {
               for (j = 0; j < n_umus_v; ++j) {
                    I_u[i_out_level][offset + j] = I_0[j];
               }
               for (j = 0; j < n_derivs; ++j) {
                    for (k = 0; k < n_umus_v; ++k) {
                         I_u_l[i_out_level][j][offset + k] = I_0_l[j][k];
                    }
               }

               i_out_level += inc;
          }
     }

}

else {

     for (i = i1; i != i2; i += inc) {

          if ((! flag && utau_output && i == ulevels[i_out_level]) || (flag && utau_output && i == ulevels[i_out_level])) {
               while (i_out_level >= 0 && i_out_level < n_ulevels && i == ulevels[i_out_level]) {
                    ptau = utaus[i_out_level];

                    if (n_derivs > 0)
                         I_u_l2 = I_u_l[i_out_level];
if (n_derivs == 0) {
     if (! solar) {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
     else {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, NULL, I_0, NULL,  offset, I_u[i_out_level], NULL,   add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,             NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
}
else {

if (! flags_or2(derivs->layers, n_layers, n_derivs)) {
     if (! solar) {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
     else {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
}
else {
     if (! solar) {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
     else {
          if (! thermal || i_four > 0)
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], NULL,      NULL,      B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
                    SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, offset, I_u[i_out_level], I_u_l2, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
}

}
                    i_out_level += inc;
               }
          }

          if (! flag)
               ptau = 0.;
          else
               ptau = ltau[i];
if (n_derivs == 0) {
     if (! solar) {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
     else {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], NULL,       ltau[i], NULL,      btran[i], NULL,       as_0[i], NULL,      atran[i], NULL,       P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,     NULL,       B, NULL, I_0, NULL,  0, I_0, NULL,  add_single_scattering, greens, solar, thermal, utau_output, NULL,             NULL,            NULL,                work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
}
else {

if (! flags_or2(derivs->layers, n_layers, n_derivs)) {
     if (! solar) {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,           NULL,                work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,            derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
     else {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else if (! flags_or2(derivs->thermal, n_layers, n_derivs))
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         NULL,         NULL,         NULL,    NULL,     NULL,     NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
    }
}

else {
     if (! solar) {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,           NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], NULL,       P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], NULL,   NULL,   At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], NULL,         P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], NULL,     NULL,     At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], NULL,           derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
     else {
          if (! thermal || i_four > 0)
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], NULL,      NULL,      B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], NULL,               work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
          else
               SFI_LAYER(i, ptau, i_four, n_quad, n_stokes, n_derivs, n_umus, qf, qx_v, qw_v, F_0, umus, omega[i], omega_l[i], ltau[i], ltau_l[i], btran[i], btran_l[i], as_0[i], as_0_l[i], atran[i], atran_l[i], P_u0_pm[i], P_uq_pp[i], P_uq_mp[i], nu[i], X_p[i], X_m[i], F_p[i], F_m[i], At_p[i], At_m[i], F0_p[i], F0_m[i], F1_p[i], F1_m[i], P_u0_pm_l[i], P_uq_pp_l[i], P_uq_mp_l[i], nu_l[i], X_p_l[i], X_m_l[i], F_p_l[i], F_m_l[i], At_p_l[i], At_m_l[i], B, B_l,  I_0, I_0_l, 0, I_0, I_0_l, add_single_scattering, greens, solar, thermal, utau_output, derivs->layers[i], derivs->beam[i], derivs->thermal[i], work, flag, c, c_l, P_q0_mm[i], P_q0_pm[i]);
     }
}

}
          if ((! flag && ! utau_output && i == ulevels[i_out_level]) || (flag && ! utau_output && i + 1 == ulevels[i_out_level])) {
               for (j = 0; j < n_umus_v; ++j) {
                    I_u[i_out_level][offset + j] = I_0[j];
               }
               for (j = 0; j < n_derivs; ++j) {
                    for (k = 0; k < n_umus_v; ++k) {
                         I_u_l[i_out_level][j][offset + k] = I_0_l[j][k];
                    }
               }

               i_out_level += inc;
          }
     }

}

}



/*******************************************************************************
 *
 ******************************************************************************/
void SFI_UP(int i_four,
            int n_quad, int n_stokes, int n_derivs, int n_layers, int n_umus,
            double qf, double *qx_v, double *qw_v, double F_0, double mu_0,
            int n_ulevels, int *ulevels, double *utaus, double *umus,
            double *omega, double **omega_l, double *ltau, double **ltau_l,
            double *Rs_u0, double **Rs_u0_l, double **Rs_uq, double ***Rs_uq_l,
            double *btran, double **btran_l,
            double *as_0, double **as_0_l, double *atran, double **atran_l,
            double **P_u0_pm,
            double ***P_uq_pp, double ***P_uq_mp, double ***P_uq_mm, double ***P_uq_pm,
            TYPE **nu, TYPE ***X_p, TYPE ***X_m,
            double **F_p, double **F_m,
            double ***At_p, double ***At_m,
            double **F0_p, double **F0_m, double **F1_p, double **F1_m,
            double ***P_u0_pm_l,
            double ****P_uq_pp_l, double ****P_uq_mp_l, double ****P_uq_mm_l, double ****P_uq_pm_l,
            TYPE ***nu_l, TYPE ****X_p_l, TYPE ****X_m_l,
            double ***F_p_l, double ***F_m_l,
            double ****At_p_l, double ****At_m_l,
            TYPE *B, TYPE **B_l,
            double **I_m, double ***K_m, double **I_u, int offset, double ***I_u_l,
            int add_single_scattering, int greens, int surface, int solar, int thermal, int utau_output,
            derivs_data *derivs, work_data work, double F_iso_bot, double *F_iso_bot_l, double surface_b, double *surface_b_l, double cc[][2], double c_l[][128][2], double **P_q0_mm, double **P_q0_pm) {

     int i;
     int ii;
     int j;
     int jj;

     int n_quad_v;
     int n_umus_v;

     double a;
     double b;
     double c;

     double *u1;

     double *I_0;
     double **I_0_l;


     n_quad_v = n_quad * n_stokes;

     n_umus_v = n_umus  * n_stokes;


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     u1  = get_work1(&work, WORK_DU);

     I_0 = get_work1(&work, WORK_DU);

     if (n_derivs > 0)
          I_0_l = get_work2(&work, WORK_DU, WORK_DERIVS_V, NULL);


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     for (i = 0; i < n_umus_v; ++i)
          I_0[i] = 0.;

     for (i = 0; i < n_derivs; ++i) {
          for (j = 0; j < n_umus_v; ++j) {
               I_0_l[i][j] = 0.;
          }
     }


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     if (i_four == 0) {
          ii = 0;
          for (i = 0; i < n_umus; ++i) {
               I_0[ii] += F_iso_bot;
               ii += n_stokes;
          }

          for (i = 0; i < n_derivs; ++i) {
               jj = 0;
               for (j = 0; j < n_umus; ++j) {
                    I_0_l[i][jj] += F_iso_bot_l[i];
                    jj += n_stokes;
               }
          }
     }


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     if (surface) {
          dm_v_mul(Rs_uq, I_m[0], n_umus_v, n_quad_v, u1);
          dvec_add(I_0, u1, I_0, n_umus_v);

          for (i = 0; i < n_derivs; ++i) {
               if (derivs->layers[n_layers][i]) {
                    dm_v_mul(Rs_uq_l[i], I_m[0], n_umus_v, n_quad_v, u1);
                    dvec_add(I_0_l[i], u1, I_0_l[i], n_umus_v);
               }
                    dm_v_mul(Rs_uq, K_m[0][i], n_umus_v, n_quad_v, u1);
                    dvec_add(I_0_l[i], u1, I_0_l[i], n_umus_v);
          }
     }

     if (surface && solar) {
if (add_single_scattering) {
          a = qf * mu_0 * F_0 / PI;

          b = a * btran[n_layers];

          dvec_scale(b, Rs_u0, u1, n_umus_v);
          dvec_add(I_0, u1, I_0, n_umus_v);

          for (i = 0; i < n_derivs; ++i) {
               if (derivs->layers[n_layers][i]) {
                    for (j = 0; j < n_umus_v; ++j)
                         I_0_l[i][j] += b * Rs_u0_l[i][j];
/*
                    dvec_scale(b, Rs_u0_l[i], u1, n_umus_v);
                    dvec_add(I_0_l[i], u1, I_0_l[i], n_umus_v);
*/

               }
               if (derivs->beam[n_layers][i]) {
                    c = a * btran_l[n_layers][i];

                    for (j = 0; j < n_umus_v; ++j)
                         I_0_l[i][j] += c * Rs_u0     [j];
/*
                    dvec_scale(c, Rs_u0,    u1, n_umus_v);
                    dvec_add(I_0_l[i], u1, I_0_l[i], n_umus_v);
*/
               }
          }
}
     }

     if (surface && thermal && i_four == 0) {
          int ii;
          int jj;
          int k;
          int kk;

          ii = 0;
          for (i = 0; i < n_umus; ++i) {
               a  = 0.;
               jj = 0;
               for (j = 0; j < n_quad; ++j) {
                    a  += Rs_uq[ii][jj];
                    jj += n_stokes;
               }
               I_0[ii] += surface_b * (1. - a);

               ii += n_stokes;
          }

          for (i = 0; i < n_derivs; ++i) {
               b = surface_b_l[i] * (1. - a);

               jj = 0;
               for (j = 0; j < n_umus; ++j) {
                    c  = 0.;
                    kk = 0;
if (derivs->layers[n_layers][i]) {
                    for (k = 0; k < n_quad; ++k) {
                         c  += Rs_uq_l[i][jj][kk];
                         kk += n_stokes;
                    }
}
                    I_0_l[i][jj] += b + surface_b * -c;

                    jj += n_stokes;
               }
          }
     }


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
#ifndef REAL
     for (i = 0; i < n_layers; ++i) {
           dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp[i], P_uq_mp[i]);

           for (j = 0; j < n_derivs; ++j) {
                if (derivs->layers[i][j]) {
                     dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp_l[i][j], P_uq_mp_l[i][j]);
                }
           }
     }
#endif

     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     SFI(i_four, n_quad, n_stokes, n_derivs, n_layers, n_umus, qf, qx_v, qw_v, F_0, n_ulevels, ulevels, utaus, umus, omega, omega_l, ltau, ltau_l, btran, btran_l, as_0, as_0_l, atran, atran_l, P_u0_pm, P_uq_pp, P_uq_mp, nu, X_p, X_m, F_p, F_m, At_p, At_m, F0_p, F0_m, F1_p, F1_m, P_u0_pm_l, P_uq_pp_l, P_uq_mp_l, nu_l, X_p_l, X_m_l, F_p_l, F_m_l, At_p_l, At_m_l, B, B_l, I_0, I_0_l, I_u, offset, I_u_l, add_single_scattering, greens, solar, thermal, utau_output, derivs, work, 0, cc, c_l, P_q0_mm, P_q0_pm);


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
#ifndef REAL
     for (i = 0; i < n_layers; ++i) {
           dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp[i], P_uq_mp[i]);

           for (j = 0; j < n_derivs; ++j) {
                if (derivs->layers[i][j]) {
                     dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp_l[i][j], P_uq_mp_l[i][j]);
                }
           }
     }
#endif
}



/*******************************************************************************
 *
 ******************************************************************************/
void SFI_DN(int i_four,
            int n_quad, int n_stokes, int n_derivs, int n_layers, int n_umus,
            double qf, double *qx_v, double *qw_v, double F_0,
            int n_ulevels, int *ulevels, double *utaus, double *umus,
            double *omega, double **omega_l, double *ltau, double **ltau_l,
            double *btran, double **btran_l,
            double *as_0, double **as_0_l, double *atran, double **atran_l,
            double **P_u0_mm,
            double ***P_uq_pp, double ***P_uq_mp, double ***P_uq_mm, double ***P_uq_pm,
            TYPE **nu, TYPE ***X_p, TYPE ***X_m,
            double **F_p, double **F_m,
            double ***At_p, double ***At_m,
            double **F0_p, double **F0_m, double **F1_p, double **F1_m,
            double ***P_u0_mm_l,
            double ****P_uq_pp_l, double ****P_uq_mp_l, double ****P_uq_mm_l, double ****P_uq_pm_l,
            TYPE ***nu_l, TYPE ****X_p_l, TYPE ****X_m_l,
            double ***F_p_l, double ***F_m_l,
            double ****At_p_l, double ****At_m_l,
            TYPE *B, TYPE **B_l,
            double **I_m, double ***K_m, double **I_u, int offset, double ***I_u_l,
            int add_single_scattering, int greens, int solar, int thermal, int utau_output,
            derivs_data *derivs, work_data work, double F_iso_top, double *F_iso_top_l, double c[][2], double c_l[][128][2], double **P_q0_mm, double **P_q0_pm) {

     int i;
     int ii;
     int j;
     int jj;

     int n_umus_v;

     double *I_0;
     double **I_0_l;


     n_umus_v = n_umus  * n_stokes;


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     I_0 = get_work1(&work, WORK_DU);

     if (n_derivs > 0)
          I_0_l = get_work2(&work, WORK_DU, WORK_DERIVS_V, NULL);


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     for (i = 0; i < n_umus_v; ++i)
          I_0[i] = 0.;

     for (i = 0; i < n_derivs; ++i) {
          for (j = 0; j < n_umus_v; ++j) {
               I_0_l[i][j] = 0.;
          }
     }


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     if (i_four == 0) {
          ii = 0;
          for (i = 0; i < n_umus; ++i) {
               I_0[ii] += F_iso_top;
               ii += n_stokes;
          }

          for (i = 0; i < n_derivs; ++i) {
               jj = 0;
               for (j = 0; j < n_umus; ++j) {
                    I_0_l[i][jj] += F_iso_top_l[i];
                    jj += n_stokes;
               }
          }
     }


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
#ifndef REAL
     for (i = 0; i < n_layers; ++i) {
           if (solar)
                dm_v_mul_D_A (n_umus, n_stokes,                   P_u0_mm[i], P_u0_mm[i]);
           dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp[i], P_uq_mp[i]);

           for (j = 0; j < n_derivs; ++j) {
                if (solar && derivs->layers[i][j])
                     dm_v_mul_D_A (n_umus, n_stokes,                   P_u0_mm_l[i][j], P_u0_mm_l[i][j]);
                if (         derivs->layers[i][j])
                     dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp_l[i][j], P_uq_mp_l[i][j]);
           }
     }
#endif

     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
     SFI(i_four, n_quad, n_stokes, n_derivs, n_layers, n_umus, qf, qx_v, qw_v, F_0, n_ulevels, ulevels, utaus, umus, omega, omega_l, ltau, ltau_l, btran, btran_l, as_0, as_0_l, atran, atran_l, P_u0_mm, P_uq_mp, P_uq_pp, nu, X_p, X_m, F_p, F_m, At_p, At_m, F0_p, F0_m, F1_p, F1_m, P_u0_mm_l, P_uq_mp_l, P_uq_pp_l, nu_l, X_p_l, X_m_l, F_p_l, F_m_l, At_p_l, At_m_l, B, B_l, I_0, I_0_l, I_u, offset, I_u_l, add_single_scattering, greens, solar, thermal, utau_output, derivs, work, 1, c, c_l, P_q0_mm, P_q0_pm);


     /*-------------------------------------------------------------------------
      *
      *-----------------------------------------------------------------------*/
#ifndef REAL
     for (i = 0; i < n_layers; ++i) {
           if (solar)
                dm_v_mul_D_A (n_umus, n_stokes,                   P_u0_mm[i], P_u0_mm[i]);
           dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp[i], P_uq_mp[i]);

           for (j = 0; j < n_derivs; ++j) {
                if (solar && derivs->layers[i][j])
                     dm_v_mul_D_A (n_umus, n_stokes,                   P_u0_mm_l[i][j], P_u0_mm_l[i][j]);
                if (         derivs->layers[i][j])
                     dmat_mul_D_A2(n_umus, n_stokes, n_quad, n_stokes, P_uq_mp_l[i][j], P_uq_mp_l[i][j]);
           }
     }
#endif
}
