/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#ifndef AZ_EBUS_RESULTS_H
#define AZ_EBUS_RESULTS_H

/**
 * @file
 * Header file for structures that aggregate results returned by all 
 * handlers of an EBus event.
 */

namespace AZ
{
    /**
     * Aggregates results returned by all handlers of an EBus event. 
     * You can use this structure to add results, apply a logical AND 
     * to results, and so on.
     *
     * @tparam T The output type of the aggregator.
     * @tparam Aggregator A function object that aggregates results. 
     * The return type must match T.
     * For examples of function objects that you can use as aggregators, 
     * see functional_basic.h.
     *
     * The following example sums the values returned by all handlers.
     * @code{.cpp}
     * EBusReduceResult<int, AZStd::plus<int>> result(0);
     * MyBus::BroadcastResult(result, &MyBus::Events::GetANumber);
     * AZ_Printf("%d", result.value);
     * @endcode
     *
     * The following example determines whether all handlers, 
     * including the latest handler, return true.
     * @code{.cpp}
     * EBusReduceResult<bool, AZStd::logical_and<bool>> result(true);
     * MyBus::BroadcastResult(result, &MyBus::Events::IsDoneDoingThing);
     * // result.value is now only true if all handlers returned true.
     * AZ_Printf("%s", result.value ? "true" : "false");
     * @endcode
     *
     */
    template<class T, class Aggregator>
    struct EBusReduceResult
    {
        /**
         * The current value, which new values will be aggregated with.
         */
        T value;

        /**
         * The function object that aggregates a new value with an existing value.
         */
        Aggregator unary;

        /**
         * Creates an instance of the class without setting an initial value or 
         * a function object to use as the aggregator.
         */
        AZ_FORCE_INLINE EBusReduceResult() {}

        /**
         * Creates an instance of the class and sets the initial value and the function 
         * object to use as the aggregator.
         * @param initialValue The initial value, which new values will be aggregated with.
         * @param aggregator A function object to aggregate the values. 
         * For examples of function objects that you can use as aggregators,
         * see functional_basic.h.
         */
        AZ_FORCE_INLINE EBusReduceResult(const T& initialValue, const Aggregator& aggregator = Aggregator())
            : value(initialValue)
            , unary(aggregator)
        { }

        /**
         * Overloads the assignment operator to aggregate a new value with  
         * the existing aggregated value.
         * @param rhs A reference to the value that will be aggregated with 
         * the existing aggregated value.
         */
        AZ_FORCE_INLINE void operator=(const T& rhs)    { value = unary(value, rhs); }

        /**
         * Overloads the assignment operator to aggregate a new value with
         * the existing aggregated value.
         * @param rhs A reference to the value that will be aggregated with
         * the existing aggregated value.
         */
        AZ_FORCE_INLINE void operator=(T& rhs)          { value = unary(value, rhs); }
    };
    /**
     * Aggregates results returned by all handlers of an EBus event. 
     * Also puts the aggregated value into the input value that is 
     * passed by reference. You can use this structure to add results, 
     * apply a logical AND to results, and so on.
     *
     * @tparam T The output type of the aggregator.
     * @tparam Aggregator A function object that aggregates results.
     * The return type must match T. 
     * For examples of function objects that you can use as aggregators,
     * see functional_basic.h.
     *
     * The following example replaces the value returned by a handler  
     * with the sum of the value and previous handler return values.
     * @code{.cpp}
     * int myExistingInt = 5;
     * EBusReduceResult<int&, AZStd::plus<int>> result(myExistingInt);
     * MyBus::BroadcastResult(result, &MyBus::Events::GetANumber);
     * AZ_Printf("%d", result.value); // or AZ_Printf("%d", myExistingInt);
     * @endcode
     *
     * The following example determines whether all handlers, 
     * including the latest handler, return true. Also replaces 
     * the latest handler result with the aggregated result.
     * @code{.cpp}
     * bool myExistingBool = true;
     * EBusReduceResult<bool&, AZStd::logical_and<bool>> result(myExistingBool);
     * MyBus::BroadcastResult(result, &MyBus::Events::IsDoneDoingThing);
     * // myExistingBool is now only true if all handlers returned true.
     * AZ_Printf("%d", myExistingBool);
     * @endcode
     */
    template<class T, class Aggregator>
    struct EBusReduceResult<T&, Aggregator>
    {
        /**
         * A reference to the current value, which new values will be aggregated with.
         */
        T& value;

        /**
         * The function object that aggregates a new value with an existing value.
         */
        Aggregator unary;
        
        /**
         * Creates an instance of the class, sets the reference to the initial value, 
         * and sets the function object to use as the aggregator.
         * @param rhs A reference to the initial value, which new values will 
         * be aggregated with.
         * @param aggregator A function object to aggregate the values.
         * For examples of function objects that you can use as aggregators,
         * see functional_basic.h.
         */
        AZ_FORCE_INLINE explicit EBusReduceResult(T& rhs, const Aggregator& aggregator = Aggregator())
            : value(rhs)
            , unary(aggregator)
        { }

        /**
         * Overloads the assignment operator to aggregate a new value with
         * the existing aggregated value.
         * @param rhs A reference to the value that will be aggregated with
         * the existing aggregated value.
         */
        AZ_FORCE_INLINE void operator=(const T& rhs)    { value = unary(value, rhs); }

        /**
         * Overloads the assignment operator to aggregate a new value with
         * the existing aggregated value.
         * @param rhs A reference to the value that will be aggregated with
         * the existing aggregated value.
         */
        AZ_FORCE_INLINE void operator=(T& rhs)          { value = unary(value, rhs); }

        /**
         * Disallows copying an EBusReduceResult object by reference.
         */
        EBusReduceResult& operator=(const EBusReduceResult&) = delete;
    };

    /// @cond EXCLUDE_DOCS
    /**
     * @deprecated Use EBusReduceResult instead.
     */
    template <class T, class Arithmetic>
    using EBusArithmericResult = EBusReduceResult<T, Arithmetic>;

    /**
     * @deprecated Use EBusReduceResult instead.
     */
    template<class T, class Operator>
    using EBusLogicalResult = EBusReduceResult<T, Operator>;
    /// @endcond

    /**
     * Collects results returned by all handlers of an EBus event. 
     * The results are collected into an AZStd::vector.
     *
     * @tparam T The return type of the handler.
     *
     * The following is an example of adding handler results to 
     * a vector of previous results:
     * @code{.cpp}
     * EBusAggregateResults<int> result;
     * MyBus::BroadcastResult(result, &MyBus::Events::GetANumber);
     * for (const int& val : result.values) { ... }
     * @endcode
     */
    template<class T>
    struct EBusAggregateResults
    {
        /**
         * A vector that contains handler results.
         */
        AZStd::vector<T> values;

        /**
         * Overloads the assignment operator to add a new result to a vector 
         * of previous results.
         * @param rhs A reference to the value that will be added to the vector.
         */
        AZ_FORCE_INLINE void operator=(const T& rhs) { values.push_back(rhs); }
    };
}

#endif //AZ_EBUS_RESULTS_H
#pragma once