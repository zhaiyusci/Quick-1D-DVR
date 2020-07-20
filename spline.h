#ifndef SPLINE_H_
#define SPLINE_H_
#include<vector>
#include<iostream>
#include<cmath>
/** @brief One-dimension cubic Hermite spline interpolation function.
 *
 * One-dimension [cubic Hermite spline](https://en.wikipedia.org/wiki/Cubic_Hermite_spline).
 */
class CubicSpline1d {
  private:
    std::vector<double> x_;  
    std::vector<double> y_;
    std::vector<double> m_; // tangent value on each point
    int N_;    // grid number
  public:
    /** @brief Loading data.
     *
     * The tangents are calculated in this function using [finite difference method](https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Finite_difference).
     * @param N length of pointwise data.
     * @param x input variables. It is required that it is sorted.
     * @param y output variables.
     * @param mi tangent on the first point.
     * @param mf tangent on the final point.
     */
    CubicSpline1d(int N, const std::vector<double>& x, const std::vector<double>& y, double mi, double mf):
      x_(x),
      y_(y),
      m_(N ,0.),
      N_(N)
  {
      for (int i =1; i!= N-1; ++i){
        // Finite difference method are used to get the tangents...
        m_[i]=0.5*( (y[i+1]-y[i])/(x[i+1]-x[i])+(y[i-1]-y[i])/(x[i-1]-x[i]));
      }
      m_[0]=mi; // tangent of the init point
      m_[N-1]=mf; // tangent of the final point

      // deal with the absolutely abnormal situation with finite difference method
      if(fabs(mi)>1.0e30) m_[0]=(y[0]-y[1])/(x[0]-x[1]);
      if(fabs(mf)>1.0e30) m_[N-1]=(y[N-2]-y[N-1])/(x[N-2]-x[N-1]);
      return;

    }
    /** @brief Perform interpolation calculation
     *
     * There are some variant, maybe.  Here, we follow wikipedia.  [The formulae](https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Interpolation_on_an_arbitrary_interval).
     */
    double operator()(double x) const{

      // find the right interval using bisec method...
      int posi, posf, posm;
      posi=1;
      posf=N_-2; // here we want the extropolate value be reasonable, as if they stay in the first or last section
      if(x>x_[posi] && x<=x_[posf]){
        posm=(posi+posf)/2; // mid point.  because they are all ints here, it is ok to do so
        for(int i=0; i!=log2(N_)+1 && posf-posi!=1; ++i){
          (x>x_[posi] && x<=x_[posm] )?  posf=posm : posi=posm;
          posm=(posi+posf)/2; 
        }
      }else{
        // if x is out of range...  warning... but do not stop
        if(x<=x_[0] || x>x_[N_-1]) { 
          std::cerr<< "Warning: Cubic spline: x out of range, x="<< x << ", min(x)=" << x_[0] <<", max(x)="<<x_[N_-1] << std::endl; 
        }
        if(x<=x_[1]) { 
          posi=0; 
          posf=1; 
        }else{ 
          posi=N_-2; 
          posf=N_-1;
        }
      }
      // cerr<< posi <<"  "<< posf <<endl;

      double t=(x-x_[posi])/(x_[posf]-x_[posi]);
      double t2=t*t;
      double t3=t2*t;
      double h00=2*t3-3*t2+1;
      double h10=t3-2*t2+t;
      double h01=-2*t3+3*t2;
      double h11=t3-t2;
      // cerr << "t   " << t<< endl;
      return h00*y_[posi]
        +h10*(x_[posf]-x_[posi])*m_[posi]
        +h01*y_[posf]
        +h11*(x_[posf]-x_[posi])*m_[posf];
    }
};

#endif // SPLINE_H_
