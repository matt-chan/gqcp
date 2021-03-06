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
#include "HamiltonianParameters/HamiltonianParameters.hpp"

#include "miscellaneous.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param ao_basis     the initial AO basis
 *  @param S            the overlap integrals
 *  @param h            the one-electron integrals H_core
 *  @param g            the two-electron integrals
 *  @param C            a transformation matrix between the current molecular orbitals and the atomic orbitals
 */
HamiltonianParameters::HamiltonianParameters(std::shared_ptr<GQCP::AOBasis> ao_basis, const GQCP::OneElectronOperator& S, const GQCP::OneElectronOperator& h, const GQCP::TwoElectronOperator& g, const Eigen::MatrixXd& C) :
    BaseHamiltonianParameters(std::move(ao_basis)),
    K (S.get_dim()),
    S (S),
    h (h),
    g (g),
    C (C)
{
    // Check if the dimensions of all matrix representations are compatible
    auto error = std::invalid_argument("The dimensions of the operators and coefficient matrix are incompatible.");

    if (this->ao_basis) {  // ao_basis is not nullptr
        if (this->K != this->ao_basis->get_number_of_basis_functions()) {
            throw error;
        }
    }

    if ((h.get_dim() != this->K) || (g.get_dim() != this->K) || (C.cols() != this->K) || (C.rows() != this->K)) {
        throw error;
    }


    if (S.get_matrix_representation().isZero(1.0e-08)) {
        throw std::invalid_argument("The underlying overlap matrix cannot be a zero matrix.");
    }
}


/**
 *  A constructor that transforms the current Hamiltonian parameters with a transformation matrix
 *
 *  @param ham_par      the current Hamiltonian parameters
 *  @param C            the transformation matrix to be applied to the current Hamiltonian parameters
 */
HamiltonianParameters::HamiltonianParameters(const GQCP::HamiltonianParameters& ham_par, const Eigen::MatrixXd& C) :
    BaseHamiltonianParameters(ham_par.ao_basis),
    K (ham_par.S.get_dim()),
    S (ham_par.S),
    h (ham_par.h),
    g (ham_par.g),
    C (ham_par.C)
{
    // We have now initialized the new Hamiltonian parameters to be a copy of the given Hamiltonian parameters, so now we will transform
    this->transform(C);
}



/*
 *  PUBLIC METHODS
 */

/**
 *  In-place transform the matrix representations of Hamiltonian parameters
 *
 *  @param T    the transformation matrix between the old and the new orbital basis, it is used as
 *      b' = b T ,
 *   in which the basis functions are collected as elements of a row vector b
 *
 *  Furthermore
 *      - the overlap matrix S now gives the overlap matrix in the new molecular orbital basis
 *      - the coefficient matrix C is updated to reflect the total transformation between the new molecular orbital basis and the initial atomic orbitals
 */
void HamiltonianParameters::transform(const Eigen::MatrixXd& T) {

    this->S.transform(T);

    this->h.transform(T);
    this->g.transform(T);

    this->C = this->C * T;  // use the correct transformation formula for subsequent transformations
}


/**
 *  In-place rotate the matrix representations of the Hamiltonian parameters
 *
 *  @param U     the unitary transformation (i.e. rotation) matrix, see transform() for how the transformation matrix between the two bases should be represented
 *
 *  Furthermore, the coefficient matrix C is updated to reflect the total transformation between the new molecular orbital basis and the initial atomic orbitals
 */
void HamiltonianParameters::rotate(const Eigen::MatrixXd& U) {

    if (!U.isUnitary(1.0e-12)) {
        throw std::invalid_argument("The given matrix is not unitary.");
    }

    this->transform(U);
}


/**
 *  Using a random rotation matrix, transform the matrix representations of the Hamiltonian parameters
 */
void HamiltonianParameters::randomRotate() {

    // Get a random unitary matrix by diagonalizing a random symmetric matrix
    Eigen::MatrixXd A_random = Eigen::MatrixXd::Random(this->K, this->K);
    Eigen::MatrixXd A_symmetric = A_random + A_random.transpose();
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> unitary_solver (A_symmetric);
    Eigen::MatrixXd U_random = unitary_solver.eigenvectors();

    this->rotate(U_random);
}


/**
 *  In-place rotate the matrix representations of the Hamiltonian parameters using a unitary Jacobi rotation matrix constructed from the Jacobi rotation parameters
 *
 *  @param jacobi_rotation_parameters       the Jacobi rotation parameters (p, q, angle) that are used to specify a Jacobi rotation: we use the (cos, sin, -sin, cos) definition for the Jacobi rotation matrix. See transform() for how the transformation matrix between the two bases should be represented
 *
 *  Furthermore the coefficient matrix C is updated to reflect the total transformation between the new molecular orbital basis and the initial atomic orbitals
 */
void HamiltonianParameters::rotate(const GQCP::JacobiRotationParameters& jacobi_rotation_parameters) {

    this->S.rotate(jacobi_rotation_parameters);
    this->h.rotate(jacobi_rotation_parameters);
    this->g.rotate(jacobi_rotation_parameters);


    // Create a Jacobi rotation matrix to transform the coefficient matrix with
    size_t K = this->h.get_dim();  // number of spatial orbitals
    auto J = GQCP::jacobiRotationMatrix(jacobi_rotation_parameters, K);
    this->C = this->C * J;
}


/**
 *  Transform the Hamiltonian parameters to the Löwdin basis (i.e. T = S^{-1/2})
 */
void HamiltonianParameters::LowdinOrthonormalize() {

    // The transformation matrix to the Löwdin basis is T = S^{-1/2}
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> saes (this->S.get_matrix_representation());
    this->transform(saes.operatorInverseSqrt());
}


/**
 *  @param D      the 1-RDM
 *  @param d      the 2-RDM
 *
 *  @return the generalized Fock matrix
 */
GQCP::OneElectronOperator HamiltonianParameters::calculateGeneralizedFockMatrix(const GQCP::OneRDM& D, const GQCP::TwoRDM& d) const {

    // Check if dimensions are compatible
    if (D.get_dim() != this->K) {
        throw std::invalid_argument("The 1-RDM is not compatible with the HamiltonianParameters.");
    }

    if (d.get_dim() != this->K) {
        throw std::invalid_argument("The 2-RDM is not compatible with the HamiltonianParameters.");
    }


    // A KISS implementation of the calculation of the generalized Fock matrix F
    Eigen::MatrixXd F = Eigen::MatrixXd::Zero(this->K, this->K);
    for (size_t p = 0; p < this->K; p++) {
        for (size_t q = 0; q < this->K; q++) {

            // One-electron part
            for (size_t r = 0; r < this->K; r++) {
                F(p,q) += h(q,r) * D(p,r);
            }

            // Two-electron part
            for (size_t r = 0; r < this->K; r++) {
                for (size_t s = 0; s < this->K; s++) {
                    for (size_t t = 0; t < this->K; t++) {
                        F(p,q) += g(q,r,s,t) * d(p,r,s,t);
                    }
                }
            }  // two-electron part

        }
    }  // F elements loop


    return GQCP::OneElectronOperator(F);
}


/**
 *  @param D      the 1-RDM
 *  @param d      the 2-RDM
 *
 *  @return the super-generalized Fock matrix
 */
GQCP::TwoElectronOperator HamiltonianParameters::calculateSuperGeneralizedFockMatrix(const GQCP::OneRDM& D, const GQCP::TwoRDM& d) const {

    // Check if dimensions are compatible
    if (D.get_dim() != this->K) {
        throw std::invalid_argument("The 1-RDM is not compatible with the HamiltonianParameters.");
    }

    if (d.get_dim() != this->K) {
        throw std::invalid_argument("The 2-RDM is not compatible with the HamiltonianParameters.");
    }


    // We have to calculate the generalized Fock matrix F first
    GQCP::OneElectronOperator F = this->calculateGeneralizedFockMatrix(D, d);

    // A KISS implementation of the calculation of the super generalized Fock matrix W
    Eigen::Tensor<double, 4> W (this->K, this->K, this->K, this->K);
    W.setZero();
    for (size_t p = 0; p < this->K; p++) {
        for (size_t q = 0; q < this->K; q++) {
            for (size_t r = 0; r < this->K; r++) {
                for (size_t s = 0; s < this->K; s++) {

                    // Generalized Fock matrix part
                    if (r == q) {
                        W(p,q,r,s) += F(p,s);
                    }

                    // One-electron part
                    W(p,q,r,s) -= this->h(s,p) * D(r,q);

                    // Two-electron part
                    for (size_t t = 0; t < this->K; t++) {
                        for (size_t u = 0; u < this->K; u++) {
                            W(p,q,r,s) += this->g(s,t,q,u) * d(r,t,p,u) - this->g(s,t,u,p) * d(r,t,u,q) - this->g(s,p,t,u) * d(r,q,t,u);
                        }
                    }  // two-electron part
                }
            }
        }
    }  // W elements loop


    return GQCP::TwoElectronOperator(W);
};



}  // namespace GQCP
