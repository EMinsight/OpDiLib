/*
 * OpDiLib, an Open Multiprocessing Differentiation Library
 *
 * Copyright (C) 2020-2021 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (opdi@scicomp.uni-kl.de)
 *
 * Lead developer: Johannes Blühdorn (SciComp, TU Kaiserslautern)
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

#include <codi.hpp>
#include <iostream>

#include <opdi/backend/macro/macroBackend.hpp>
#include <codi/tools/parallel/openmp/codiOpenMP.hpp>
#include <codi/tools/parallel/openmp/codiOpDiLibTool.hpp>
#include <opdi.hpp>

using Real = codi::RealReverseIndexOpenMP;
using Tape = typename Real::Tape;

int main(int nargs, char** args) {

  // initialize OpDiLib

  opdi::backend = new opdi::MacroBackend();
  opdi::backend->init();
  opdi::logic = new opdi::OmpLogic;
  opdi::logic->init();
  opdi::tool = new CoDiOpDiLibTool<Real>;

  // initialize thread-safe version of CoDiPack

  Tape& tape = Real::getTape();
  tape.initialize();

  // usual AD workflow

  Real x = 4.0;

  tape.setActive();
  tape.registerInput(x);

  // parallel computation

  Real a[1000];
  Real y = 0.0;

  OPDI_PARALLEL()
  {
    OPDI_FOR()
    for (int i = 0; i < 1000; ++i)
    {
      a[i] = sin(x * i);
    }
    OPDI_END_FOR
  }
  OPDI_END_PARALLEL

  for (int i = 0; i < 1000; ++i) {
    y += a[i];
  }

  // usual AD workflow

  tape.registerOutput(y);
  tape.setPassive();
  y.setGradient(1.0);
  tape.evaluate();

  std::cout << "f(" << x << ") = " << y << std::endl;
  std::cout << "df/dx(" << x << ") = " << x.getGradient() << std::endl;

  // finalize OpDiLib

  opdi::backend->finalize();
  delete opdi::backend;
  delete opdi::logic;
  delete opdi::tool;

  return 0;
}

// don't forget to include the OpDiLib source file
#include "opdi.cpp"

