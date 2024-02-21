//
// Created by Monika on 20.02.2024.
//

#ifndef SR_UTILS_NETWORK_CONTEXT_H
#define SR_UTILS_NETWORK_CONTEXT_H

#include <Utils/Common/Enumerations.h>
#include <Utils/Types/SharedPtr.h>

namespace SR_NETWORK_NS {
    class Socket;
    class Acceptor;
    class PeerToPeer;

    SR_ENUM_NS_CLASS_T(NetworkLib, uint8_t,
        Unknown,
        Asio
    )

    SR_ENUM_NS_CLASS_T(SocketType, uint8_t,
        Unknown,
        TCP,
        UDP
    )

    class Context : public SR_HTYPES_NS::SharedPtr<Context> {
        using Super = SR_HTYPES_NS::SharedPtr<Context>;
    protected:
        Context();

    public:
        virtual ~Context() = default;

    public:
        SR_NODISCARD static SR_HTYPES_NS::SharedPtr<Context> Create();

        virtual bool Run() = 0;
        virtual void Stop() = 0;

        virtual bool Poll();

    public:
        void AddAsyncAcceptor(const SR_HTYPES_NS::SharedPtr<Acceptor>& pAcceptor);

        SR_NODISCARD virtual SR_HTYPES_NS::SharedPtr<Socket> CreateSocket(SocketType type) = 0;
        SR_NODISCARD virtual SR_HTYPES_NS::SharedPtr<Acceptor> CreateAcceptor(SocketType type, const std::string& address, uint16_t port) = 0;

        SR_NODISCARD SR_HTYPES_NS::SharedPtr<PeerToPeer> CreateP2P(SocketType type, const std::string& address, uint16_t port);

    protected:
        bool m_isRunning = false;

        std::list<SR_HTYPES_NS::SharedPtr<Acceptor>> m_asyncAcceptors;

    };
}

#endif //SR_UTILS_NETWORK_CONTEXT_H
