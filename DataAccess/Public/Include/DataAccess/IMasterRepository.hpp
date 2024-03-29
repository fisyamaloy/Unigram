#pragma once
#include <iostream>
#include <memory>

namespace DataAccess
{
/**
 * @brief The IMasterRepository base abstract class
 */
struct IMasterRepository
{
    /// Default virtual destructor
    virtual ~IMasterRepository() = default;
};

}  // namespace DataAccess
