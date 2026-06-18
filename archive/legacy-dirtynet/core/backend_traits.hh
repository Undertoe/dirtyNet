#pragma once
#include "backend_tags.hh"

namespace dirtyNet::detail
{
    class BlockingContext{
    public:
        BlockingContext();
        ~BlockingContext();

        // this is where our read / writes should sit
    };

    class AsyncContext{
    public:
        AsyncContext();
        ~AsyncContext();

        // this is where our read / writes should sit
    };


    template<class BackendTag>
    struct BackendTraits;

    template<>
    struct BackendTraits<dirtyNet::Blocking>{
        using ContextType = BlockingContext;

        static ContextType& context(){
            static ContextType context;
            return context;
        }   
    };


    template<>
    struct BackendTraits<dirtyNet::Async>{
        using ContextType = AsyncContext;

        static ContextType& context(){
            static ContextType context;
            return context;
        }   
    };
}