/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2008 Yee Man Chan

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/models/equity/gjrgarchmodel.hpp>
#include <ql/quotes/simplequote.hpp>

namespace QuantLib {

    class GJRGARCHModel::VolatilityConstraint : public Constraint {
      private:
        class Impl : public Constraint::Impl {
          public:
            bool test(const Array& params) const {
                const Real omega = params[0];
                const Real alpha = params[1];
                const Real beta = params[2];
                const Real gamma = params[3];

                return (omega > 0.0 && alpha >= 0.0
                        && beta >= 0.0 && alpha+gamma >= 0.0);
            }
        };
      public:
        VolatilityConstraint()
        : Constraint(boost::shared_ptr<Constraint::Impl>(
                                           new VolatilityConstraint::Impl)) {}
    };

    GJRGARCHModel::GJRGARCHModel(
                           const boost::shared_ptr<GJRGARCHProcess> & process)
    : CalibratedModel(6), process_(process) {
        arguments_[0] = ConstantParameter(process->omega(),
                                          PositiveConstraint());
        arguments_[1] = ConstantParameter(process->alpha(),
                                          PositiveConstraint());
        arguments_[2] = ConstantParameter(process->beta(),
                                          PositiveConstraint());
        arguments_[3] = ConstantParameter(process->gamma(),
                                          BoundaryConstraint(-1.0, 1.0));
        arguments_[4] = ConstantParameter(process->lambda(),
                                          BoundaryConstraint(-1.0, 1.0));
        arguments_[5] = ConstantParameter(process->v0(),
                                          PositiveConstraint());
        generateArguments();

        registerWith(process_->riskFreeRate());
        registerWith(process_->dividendYield());
        registerWith(process_->s0());
    }

    void GJRGARCHModel::generateArguments() {
        process_.reset(new GJRGARCHProcess(process_->riskFreeRate(),
                                           process_->dividendYield(),
                                           process_->s0(),
                                           v0(), omega(), alpha(),
                                           beta(), gamma(), lambda()));
    }
}
