#pragma once

#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <vector>

namespace d2d { namespace util {

  template<typename type>
  using pair = std::array<type, 2>;

  template<typename type>
  using triple = std::array<type, 3>;

  template<typename numeric_type>
  triple<numeric_type>
  sum (const triple<numeric_type>& pF, const triple<numeric_type>& pS)
  {
    return {pF[0] + pS[0], pF[1] + pS[1], pF[2] + pS[2]};
  }

  // template<typename numeric_type>
  // triple<numeric_type>
  // sum
  // (const triple<numeric_type>& pF,
  //  const triple<numeric_type>& pS,
  //  const triple<numeric_type>& pT)
  // {
  //   return {pF[0] + pS[0] + pT[0],
  //           pF[1] + pS[1] + pT[1],
  //           pF[2] + pS[2] + pT[2]};
  // }

  template<typename numeric_type>
  triple<numeric_type>
  inv (const triple<numeric_type>& pT)
  {
    return {-pT[0], -pT[1], -pT[2]};
  }

  template<typename numeric_type>
  triple<numeric_type>
  diff(const triple<numeric_type>& pF, const triple<numeric_type>& pS)
  {
    return sum(pF, inv(pS));
  }

  template<typename numeric_type>
  triple<numeric_type>
  cross_product
  (const triple<numeric_type>& pF, const triple<numeric_type>& pS)
  {
    triple<numeric_type> rr;
    rr[0] = pF[1] * pS[2] - pF[2] * pS[1];
    rr[1] = pF[2] * pS[0] - pF[0] * pS[2];
    rr[2] = pF[0] * pS[1] - pF[1] * pS[0];
    return rr;
  }

  // Compute normal of a triangle
  template<typename numeric_type>
  triple<numeric_type>
  compute_normal (triple<triple<numeric_type> >& pTri)
  {
    auto uu = diff(pTri[1], pTri[0]);
    auto vv = diff(pTri[2], pTri[0]);
    return cross_product(uu, vv);
  }

  template<typename numeric_type>
  bool is_normalized(triple<numeric_type> const& pV)
  {
    auto epsilon = 1e-6f;
    numeric_type length =
      std::sqrt(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2]);
    return 1-epsilon <= length && length <= 1+epsilon;
  }

  // This function modifies its argument when called
  template<typename numeric_type>
  void normalize(triple<numeric_type>& pV)
  {
    numeric_type thrdNorm =
      std::sqrt(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2]);
    pV[0] /= thrdNorm;
    pV[1] /= thrdNorm;
    pV[2] /= thrdNorm;
    if ( ! is_normalized(pV) )
      std::cerr
        << "Warning: Assertion error is about to happen. thrdNorm == "
        << thrdNorm << std::endl;
    assert( is_normalized(pV) && "Postcondition" );
  }

  // Input: a triple of triples where each inner triple holds the x, y
  // and z coordinates of a vertex of a triangle.
  template<typename numeric_type>
  static triple<numeric_type>
  get_centroid(triple<triple<numeric_type> > pTriangle)
  {
    triple<numeric_type> result;
    result[0] = (pTriangle[0][0] + pTriangle[1][0] + pTriangle[2][0]) / 3;
    result[1] = (pTriangle[0][1] + pTriangle[1][1] + pTriangle[2][1]) / 3;
    result[2] = (pTriangle[0][2] + pTriangle[1][2] + pTriangle[2][2]) / 3;
    return result;
  }

  template<typename numeric_type>
  static numeric_type
  distance(triple<numeric_type> ip1, triple<numeric_type> ip2)
  {
    auto p1 = ip1[0] - ip2[0];
    auto p2 = ip1[1] - ip2[1];
    auto p3 = ip1[2] - ip2[2];
    return std::sqrt(p1*p1 + p2*p2 + p3*p3);
  }

  template<typename numeric_type>
  static numeric_type
  distance(pair<triple<numeric_type> >& pPnts)
  {
    return distance(pPnts[0], pPnts[1]);
  }

  // A c-style-array foldl as generic auxiliary implementation.
  // This implementation should be reasonably efficient. It tries to
  // minimize copies of data.
  template<typename T1, typename T2>
  static T1& foldl_aux(std::function<T1 (T1&, T2 const&)>& pF,
                       T1& pT1,
                       T2 const* pT2,
                       size_t& pT2Length)
  {
    if (pT2Length <= 0) return pT1;
    // apply the fold-function, modify the content of pT1
    pT1 = pF(pT1, pT2[0]);
    return foldl_aux<T1, T2>(pF, pT1, &pT2[1], --pT2Length /* modify pT2Length */);
  }

  // A foldl for std::vector
  template<typename T1, typename T2>
  static T1 foldl(std::function<T1 (T1&, T2 const&)> pF,
                  T1 pT1,
                  std::vector<T2> pT2)
  {
    // copy the size once into a separate memory location.
    auto pT2Size = pT2.size();
    return foldl_aux<T1,T2>(pF, pT1, pT2.data(), pT2Size);
  }
}}
