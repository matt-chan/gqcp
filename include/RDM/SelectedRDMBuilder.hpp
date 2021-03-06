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
#ifndef GQCP_SELECTEDRDMBUILDER_HPP
#define GQCP_SELECTEDRDMBUILDER_HPP


#include "FockSpace/SelectedFockSpace.hpp"
#include "RDM/BaseRDMBuilder.hpp"
#include "RDM/RDMs.hpp"


namespace GQCP {


/**
 *  A class capable of calculating 1- and 2-RDMs from wave functions expanded in a selected Fock space
 */
class SelectedRDMBuilder : public BaseRDMBuilder {
    SelectedFockSpace fock_space;  // Fock space containing the selected configurations


public:
    // CONSTRUCTORS
    explicit SelectedRDMBuilder (const SelectedFockSpace& fock_space);


    // DESTRUCTOR
    ~SelectedRDMBuilder() = default;


    // OVERRIDDEN GETTERS
    BaseFockSpace* get_fock_space() override { return &fock_space; }


    // OVERRIDDEN PUBLIC METHODS
    /**
     *  @param x        the coefficient vector representing the 'selected' wave function
     *
     *  @return all 1-RDMs given a coefficient vector
     */
    OneRDMs calculate1RDMs(const Eigen::VectorXd& x) override;

    /**
     *  @param x        the coefficient vector representing the 'selected' wave function
     *
     *  @return all 2-RDMs given a coefficient vector
     */
    TwoRDMs calculate2RDMs(const Eigen::VectorXd& x) override;
};


}  // namespace GQCP


#endif  // GQCP_SELECTEDRDMBUILDER_HPP
