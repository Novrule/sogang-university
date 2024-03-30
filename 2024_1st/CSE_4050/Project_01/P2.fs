module P2

// (Note) Do NOT change the definition of the following type and exception.
type Exp =
    Num of int
  | Add of Exp * Exp
  | Sub of Exp * Exp
  | Mul of Exp * Exp
  | Div of Exp * Exp

exception DivByZero

/// Return the integer value represented by the expression 'e'. If there is any
/// division-by-zero case, raise the 'DivByZero' exception.
let rec eval (e: Exp) : int =
  match e with
    | Num n -> n
    | Add (e1, e2) -> eval e1 + eval e2
    | Sub (e1, e2) -> eval e1 - eval e2
    | Mul (e1, e2) -> eval e1 * eval e2
    | Div (e1, e2) ->
      let n2 = eval e2
      if n2 <> 0 
        then 
          let n1 = eval e1
          n1 / n2
        else
          raise DivByZero
