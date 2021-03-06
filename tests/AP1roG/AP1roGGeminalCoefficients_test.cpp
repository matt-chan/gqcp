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
#define BOOST_TEST_MODULE "AP1roGGeminalCoefficients"


#include "AP1roG/AP1roGGeminalCoefficients.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>  // include this to get main(), otherwise the compiler will complain


BOOST_AUTO_TEST_CASE ( numberOfGeminalCoefficients ) {

    BOOST_CHECK_EQUAL(GQCP::AP1roGGeminalCoefficients::numberOfGeminalCoefficients(2, 5), 6);

    BOOST_CHECK_THROW(GQCP::AP1roGGeminalCoefficients::numberOfGeminalCoefficients(4, 4), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE ( default_constructor ) {
    GQCP::AP1roGGeminalCoefficients gem_coeff;
}


BOOST_AUTO_TEST_CASE ( constructor ) {

    // Check a correct constructor
    GQCP::AP1roGGeminalCoefficients g (4, 6);

    // We can't create 4 geminals in 4 orbitals
    BOOST_CHECK_THROW(GQCP::AP1roGGeminalCoefficients (4, 4), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE ( constructor_vector ) {

    // Check a correct constructor
    Eigen::VectorXd g = Eigen::VectorXd::Zero(6);
    BOOST_CHECK_NO_THROW(GQCP::AP1roGGeminalCoefficients (g, 2, 5));

    // Check wrong parameters N_P and K
    BOOST_CHECK_THROW(GQCP::AP1roGGeminalCoefficients (g, 2, 4), std::invalid_argument);
    BOOST_CHECK_THROW(GQCP::AP1roGGeminalCoefficients (g, 1, 5), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE ( asVector ) {

    GQCP::AP1roGGeminalCoefficients g (4, 6);
    g.asVector();
}


BOOST_AUTO_TEST_CASE ( asMatrix ) {

    // For N_P=2 and K=5, we have an AP1roG geminal coefficient matrix that looks like the following matrix:
    Eigen::MatrixXd G (2, 5);
    G << 1, 0,  1, 2, 3,
         0, 1,  4, 5, 6;

    // The geminal coefficients, arranged in a vector are then represented by the following vector:
    Eigen::VectorXd g (6);
    g << 1, 2, 3, 4, 5, 6;


    GQCP::AP1roGGeminalCoefficients gem_coeff (g, 2, 5);
    BOOST_CHECK(gem_coeff.asMatrix().isApprox(G));
}


BOOST_AUTO_TEST_CASE ( vector_index ) {

    // N_P=2, K=11
    GQCP::AP1roGGeminalCoefficients geminal_coefficients (2, 11);

    BOOST_CHECK_EQUAL(geminal_coefficients.vectorIndex(0, 2), 0);
    BOOST_CHECK_EQUAL(geminal_coefficients.vectorIndex(0, 3), 1);
    BOOST_CHECK_EQUAL(geminal_coefficients.vectorIndex(1, 2), 9);
    BOOST_CHECK_EQUAL(geminal_coefficients.vectorIndex(1, 3), 10);

    // Require a throw if i > N_P
    BOOST_REQUIRE_THROW(geminal_coefficients.vectorIndex(3, 3), std::invalid_argument);

    // Require a throw if a < N_P
    BOOST_REQUIRE_THROW(geminal_coefficients.vectorIndex(0, 1), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE ( matrix_index ) {

    // N_P=2, K=11
    GQCP::AP1roGGeminalCoefficients geminal_coefficients (2, 11);


    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMajor(0), 0);
    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMajor(1), 0);
    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMajor(9), 1);
    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMajor(10), 1);

    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMinor(0), 2);
    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMinor(1), 3);
    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMinor(4), 6);
    BOOST_CHECK_EQUAL(geminal_coefficients.matrixIndexMinor(5), 7);
}


BOOST_AUTO_TEST_CASE ( operator_call ) {
    // Make an example for geminal coefficients for N_P=2 and K=5
    //      1 0  1 2 3
    //      0 1  4 5 6
    Eigen::VectorXd g (6);
    g << 1, 2, 3, 4, 5, 6;
    GQCP::AP1roGGeminalCoefficients gem_coeff (g, 2, 5);

    BOOST_CHECK_EQUAL(gem_coeff(0), 1);
    BOOST_CHECK_EQUAL(gem_coeff(1), 2);
    BOOST_CHECK_EQUAL(gem_coeff(2), 3);
    BOOST_CHECK_EQUAL(gem_coeff(3), 4);
    BOOST_CHECK_EQUAL(gem_coeff(4), 5);
    BOOST_CHECK_EQUAL(gem_coeff(5), 6);


    BOOST_CHECK_EQUAL(gem_coeff(0, 2), 1);
    BOOST_CHECK_EQUAL(gem_coeff(0, 3), 2);
    BOOST_CHECK_EQUAL(gem_coeff(0, 4), 3);
    BOOST_CHECK_EQUAL(gem_coeff(1, 2), 4);
    BOOST_CHECK_EQUAL(gem_coeff(1, 3), 5);
    BOOST_CHECK_EQUAL(gem_coeff(1, 4), 6);
}
