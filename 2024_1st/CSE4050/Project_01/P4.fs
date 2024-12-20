module P4

/// The summation operator (Σ). "sigma f a b" must return "f a + f (a + 1) +
/// ... + f b" as a result. If a > b, then return 0.
let rec helper (f: int -> int) (a: int) (b: int) (sum: int) : int =
  if a > b
    then
      sum
    else
      helper f (a + 1) b (sum + f a)      
      
let rec sigma (f: int -> int) (a: int) (b: int) : int =
  helper f a b 0
