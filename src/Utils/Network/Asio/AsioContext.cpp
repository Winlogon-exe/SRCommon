//
// Created by Monika on 20.02.2024.
//

#include <Utils/Network/Asio/AsioContext.h>
#include <Utils/Network/Asio/AsioTCPSocket.h>
#include <Utils/Network/Asio/AsioTCPAcceptor.h>

namespace SR_NETWORK_NS {
    SR_HTYPES_NS::SharedPtr<Socket> AsioContext::CreateSocket(SocketType type) {
        switch (type) {
            case SocketType::TCP:
                return new AsioTCPSocket(GetThis());
            case SocketType::UDP:
                // return new AsioUDPSocket(GetThis());
            default:
                SR_ERROR("AsioContext::CreateSocket() : unknown socket type: {}", SR_UTILS_NS::EnumReflector::ToStringAtom(type).c_str());
                return nullptr;
        }
    }

    SR_HTYPES_NS::SharedPtr<Acceptor> AsioContext::CreateAcceptor(SocketType type, const std::string& address, uint16_t port) {
        switch (type) {
            case SocketType::TCP: {
                auto&& pAcceptor = new AsioTCPAcceptor(GetThis(), address, port);
                return pAcceptor;
            }
            case SocketType::UDP:
                // return new AsioUDPAcceptor(GetThis(), address, port);
            default:
                SR_ERROR("AsioContext::CreateAcceptor() : unknown socket type: {}", SR_UTILS_NS::EnumReflector::ToStringAtom(type).c_str());
                return nullptr;
        }
    }

    bool AsioContext::Run() {
        asio::error_code errorCode;

        m_context.run(errorCode);

        if (errorCode) {
            SR_ERROR("AsioContext::Run() : failed to run context: {}", errorCode.message());
            return false;
        }

        return true;
    }

    bool AsioContext::Pool() {
        asio::error_code errorCode;

        m_context.poll(errorCode);

        if (errorCode) {
            SR_ERROR("AsioContext::Pool() : failed to pool context: {}", errorCode.message());
            return false;
        }

        return true;
    }
}