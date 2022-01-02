#include "Exception.hpp"

namespace Tixel::Infra
{

CmdLineException::CmdLineException(const std::string& message): std::invalid_argument(message.c_str())
{}

TransactionException::TransactionException(const std::string& message) : std::runtime_error(message.c_str())
{}

void CHECK_CMDLINE(bool condition, const std::string& message)
{
	if (!condition)
	{
		throw CmdLineException(message);
	}
}

void ROLLBACK_TXN(bool condition, const std::string& message)
{
	if (!condition)
	{
		throw TransactionException(message);
	}
}

}
