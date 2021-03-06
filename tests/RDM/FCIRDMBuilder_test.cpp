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
#define BOOST_TEST_MODULE "FCI_RDM_test"

#include "RDM/RDMCalculator.hpp"

#include "CISolver/CISolver.hpp"
#include "HamiltonianBuilder/FCI.hpp"
#include "HamiltonianParameters/HamiltonianParameters_constructors.hpp"
#include "properties/expectation_values.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE ( H2O_1RDM_spin_trace_FCI ) {

    // Test if the trace of the 1-RDMs (spin summed, aa and bb) gives N, N_a and N_b

    // Get the 1-RDM from FCI
    size_t N_a = 5;
    size_t N_b = 5;

    // Create a Molecule and an AOBasis
    GQCP::Molecule h2o ("../tests/data/h2o_Psi4_GAMESS.xyz");
    auto ao_basis = std::make_shared<GQCP::AOBasis>(h2o, "STO-3G");

    // Create the molecular Hamiltonian parameters for this molecule and basis
    auto ham_par = GQCP::constructMolecularHamiltonianParameters(ao_basis);
    size_t K = ham_par.get_K();  // SO 7

    GQCP::ProductFockSpace fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, ham_par);
    numopt::eigenproblem::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    Eigen::VectorXd coef = ci_solver.get_eigenpair().get_eigenvector();

    // Check if the FCI 1-RDMs have the proper trace.
    GQCP::RDMCalculator fci_rdm (fock_space);
    GQCP::OneRDMs one_rdms = fci_rdm.calculate1RDMs(coef);


    BOOST_CHECK(std::abs(one_rdms.one_rdm_aa.trace() - N_a) < 1.0e-12);
    BOOST_CHECK(std::abs(one_rdms.one_rdm_bb.trace() - N_b) < 1.0e-12);
    BOOST_CHECK(std::abs(one_rdms.one_rdm.trace() - (N_a + N_b)) < 1.0e-12);
}


BOOST_AUTO_TEST_CASE ( H2O_2RDM_spin_trace_FCI ) {

    // Test if the traces of the spin-resolved 2-RDMs (d_ppqq) gives the correct number
    
    size_t N_a = 5;
    size_t N_b = 5;

    // Create a Molecule and an AOBasis
    GQCP::Molecule h2o ("../tests/data/h2o_Psi4_GAMESS.xyz");
    auto ao_basis = std::make_shared<GQCP::AOBasis>(h2o, "STO-3G");

    // Create the molecular Hamiltonian parameters for this molecule and basis
    auto ham_par = GQCP::constructMolecularHamiltonianParameters(ao_basis);
    size_t K = ham_par.get_K();  // SO 7

    GQCP::ProductFockSpace fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, ham_par);
    numopt::eigenproblem::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    Eigen::VectorXd coef = ci_solver.get_eigenpair().get_eigenvector();

    // Check if the FCI 2-RDMs have the proper trace.
    GQCP::RDMCalculator fci_rdm (fock_space);
    GQCP::TwoRDMs two_rdms = fci_rdm.calculate2RDMs(coef);


    BOOST_CHECK(std::abs(two_rdms.two_rdm_aaaa.trace() - N_a*(N_a-1)) < 1.0e-12);
    BOOST_CHECK(std::abs(two_rdms.two_rdm_aabb.trace() - N_a*N_b) < 1.0e-12);
    BOOST_CHECK(std::abs(two_rdms.two_rdm_bbaa.trace() - N_b*N_a) < 1.0e-12);
    BOOST_CHECK(std::abs(two_rdms.two_rdm_bbbb.trace() - N_b*(N_b-1)) < 1.0e-12);
}


BOOST_AUTO_TEST_CASE ( H2O_1RDM_2RDM_trace_FCI ) {

    // Test if the relevant 2-RDM trace gives the 1-RDM for FCI

    size_t N_a = 5;
    size_t N_b = 5;
    size_t N = N_a + N_b;
    // Create a Molecule and an AOBasis
    GQCP::Molecule h2o ("../tests/data/h2o_Psi4_GAMESS.xyz");
    auto ao_basis = std::make_shared<GQCP::AOBasis>(h2o, "STO-3G");

    // Create the molecular Hamiltonian parameters for this molecule and basis
    auto ham_par = GQCP::constructMolecularHamiltonianParameters(ao_basis);
    size_t K = ham_par.get_K();  // SO 7

    GQCP::ProductFockSpace fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, ham_par);
    numopt::eigenproblem::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    Eigen::VectorXd coef = ci_solver.get_eigenpair().get_eigenvector();

    // Check if the 2-RDM contraction matches the reduction.
    GQCP::RDMCalculator fci_rdm (fock_space);
    GQCP::TwoRDMs two_rdms = fci_rdm.calculate2RDMs(coef);
    GQCP::OneRDMs one_rdms = fci_rdm.calculate1RDMs(coef);


    Eigen::MatrixXd D_from_reduction = (1.0/(N-1)) * two_rdms.two_rdm.reduce();
    BOOST_CHECK(one_rdms.one_rdm.get_matrix_representation().isApprox(D_from_reduction, 1.0e-12));
}


BOOST_AUTO_TEST_CASE ( H2O_energy_RDM_contraction_FCI ) {

    // Test if the contraction of the 1- and 2-RDMs with the one- and two-electron integrals gives the FCI energy

    size_t N_a = 5;
    size_t N_b = 5;
    // Create a Molecule and an AOBasis
    GQCP::Molecule h2o ("../tests/data/h2o_Psi4_GAMESS.xyz");
    auto ao_basis = std::make_shared<GQCP::AOBasis>(h2o, "STO-3G");

    // Create the molecular Hamiltonian parameters for this molecule and basis
    auto ham_par = GQCP::constructMolecularHamiltonianParameters(ao_basis);
    size_t K = ham_par.get_K();  // SO 7

    GQCP::ProductFockSpace fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, ham_par);
    numopt::eigenproblem::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    Eigen::VectorXd coef = ci_solver.get_eigenpair().get_eigenvector();
    double energy_by_eigenvalue = ci_solver.get_eigenpair().get_eigenvalue();

    // Check if the contraction energy matches the fci eigenvalue.
    GQCP::RDMCalculator fci_rdm (fock_space);
    GQCP::TwoRDMs two_rdms = fci_rdm.calculate2RDMs(coef);
    GQCP::OneRDMs one_rdms = fci_rdm.calculate1RDMs(coef);

    double energy_by_contraction = GQCP::calculateExpectationValue(ham_par, one_rdms.one_rdm, two_rdms.two_rdm);

    BOOST_CHECK(std::abs(energy_by_eigenvalue - energy_by_contraction) < 1.0e-12);
}
