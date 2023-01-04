﻿/*
 * OpDiLib, an Open Multiprocessing Differentiation Library
 *
 * Copyright (C) 2020-2022 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Copyright (C) 2023 Chair for Scientific Computing (SciComp), University of Kaiserslautern-Landau
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (opdi@scicomp.uni-kl.de)
 *
 * Lead developer: Johannes Blühdorn (SciComp, University of Kaiserslautern-Landau)
 *
 * This file is part of OpDiLib (http://www.scicomp.uni-kl.de/software/opdi).
 *
 * OpDiLib is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * OpDiLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with OpDiLib.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * For other licensing options please contact us.
 *
 */

#pragma once

#include "testBase.hpp"

template<typename _Case>
struct TestSingleNowait : public TestBase<4, 1, 3, TestSingleNowait<_Case>> {
  public:
    using Case = _Case;
    using Base = TestBase<4, 1, 3, TestSingleNowait<Case>>;

    template<typename T>
    static void test(std::array<T, Base::nIn> const& in, std::array<T, Base::nOut>& out) {

      int const N = 1000;
      T* jobResults = new T[N];

      OPDI_PARALLEL()
      {
        int nThreads = omp_get_num_threads();
        int start = ((N / 2 - 1) / nThreads + 1) * omp_get_thread_num();
        int end = ((N / 2 - 1) / nThreads + 1) * (omp_get_thread_num() + 1);

        for (int i = start; i < end; ++i) {
          Base::job1(i, in, jobResults[i]);
        }

        OPDI_BARRIER()

        OPDI_SINGLE_NOWAIT()
        {
          for (int i = 0; i < N / 2; ++i) {
            out[0] += jobResults[i];
          }
        }
        OPDI_END_SINGLE

        start = N / 2 + ((N / 2 - 1) / nThreads + 1) * omp_get_thread_num();
        end = std::min(N, N / 2 + ((N / 2 - 1) / nThreads + 1) * (omp_get_thread_num() + 1));

        for (int i = start; i < end; ++i) {
          Base::job1(i, in, jobResults[i]);
        }

        OPDI_BARRIER()

        OPDI_SINGLE_NOWAIT()
        {
          for (int i = N / 2; i < N; ++i) {
            out[0] += jobResults[i];
          }
        }
        OPDI_END_SINGLE
      }
      OPDI_END_PARALLEL

      delete [] jobResults;
    }
};
