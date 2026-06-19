#ifndef __CONTRACTS_H_INCL__
#define __CONTRACTS_H_INCL__

#pragma pack(push, 8)

/// \brief Require that \p expr is true at function entry; return void if false.
///
/// MUST be placed at the very top of a void function or method, before any
/// other statement. Use exclusively to guard input parameters on entry.
/// Do NOT use mid-function as general-purpose control flow.
/// If the expression evaluates to false the function returns immediately,
/// performing no work and causing no side effects.
///
/// \param expr Boolean expression representing the precondition.
///
/// Usage example:
/// \code
/// void SetTitle(const std::string& title)
/// {
///     Require(title.empty() == false);
///     // ... safe to use title here
/// }
/// \endcode
#define Require(expr) \
    do { if ((expr) == false) { return; } } while (false)

/// \brief Require that \p expr is true at function entry; return \p retval if false.
///
/// MUST be placed at the very top of a non-void function or method, before any
/// other statement. Use exclusively to guard input parameters on entry.
/// Do NOT use mid-function as general-purpose control flow.
/// If the expression evaluates to false the function returns \p retval immediately,
/// performing no work and causing no side effects.
///
/// \param expr   Boolean expression representing the precondition.
/// \param retval Value returned to the caller when the precondition is not met.
///
/// Usage example:
/// \code
/// std::string GetTitle(int index) const
/// {
///     RequireReturn(index >= 0, std::string{});
///     // ... safe to use index here
/// }
/// \endcode
#define RequireReturn(expr, retval) \
    do { if ((expr) == false) { return (retval); } } while (false)

#pragma pack(pop)

#endif // __CONTRACTS_H_INCL__
