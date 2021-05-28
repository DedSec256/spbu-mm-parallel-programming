using System;

namespace ThreadPool
{
    public class Maybe<T>
    {
        private readonly T _just;
        private readonly bool _isJust;

        public Maybe(T just)
        {
            _just = just;
            _isJust = true;
        }

        public Maybe()
        {
            _isJust = false;
        }

        public TR Match<TR>(Func<T, TR> justFunc, Func<TR> nothingFunc)
            => _isJust ? justFunc(_just) : nothingFunc();
        
    }
}