// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2018  the GQCG developers
// 
// GQCG-gqcp is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GQCG-gqcp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-gqcp.  If not, see <http://www.gnu.org/licenses/>.
// 
#define BOOST_TEST_MODULE "DOCI"


#include "HamiltonianBuilder/DOCI.hpp"
#include "HamiltonianParameters/HamiltonianParameters_constructors.hpp"
#include "RHF/PlainRHFSCFSolver.hpp"
#include <numopt.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>  // include this to get main(), otherwise the compiler will complain



BOOST_AUTO_TEST_CASE ( DOCI_constructor ) {
    // Create a compatible Fock space
    GQCP::FockSpace fock_space (15, 3);

    // Check if a correct constructor works
    BOOST_CHECK_NO_THROW(GQCP::DOCI doci (fock_space));
}


BOOST_AUTO_TEST_CASE ( DOCI_public_methods ) {

    // Create random HamiltonianParameters to check compatibility
    size_t K = 5;
    auto random_hamiltonian_parameters = GQCP::constructRandomHamiltonianParameters(K);

    // Create a compatible Fock space
    GQCP::FockSpace fock_space (K, 3);

    // Create DOCI module
    GQCP::DOCI random_doci (fock_space);

    // Test the public DOCI methods
    Eigen::VectorXd x = random_doci.calculateDiagonal(random_hamiltonian_parameters);
    BOOST_CHECK_NO_THROW(random_doci.constructHamiltonian(random_hamiltonian_parameters));
    BOOST_CHECK_NO_THROW(random_doci.matrixVectorProduct(random_hamiltonian_parameters, x, x));

    // Create an incompatible Fock space
    GQCP::FockSpace fock_space_i (K+1, 3);

    // Create DOCI module
    GQCP::DOCI random_doci_i (fock_space_i);
    BOOST_CHECK_THROW(random_doci_i.constructHamiltonian(random_hamiltonian_parameters), std::invalid_argument);
    BOOST_CHECK_THROW(random_doci_i.matrixVectorProduct(random_hamiltonian_parameters, x, x), std::invalid_argument);
}
