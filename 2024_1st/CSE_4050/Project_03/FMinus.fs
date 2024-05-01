module FMinus

open AST
open Types

// Evaluate expression into a value, under the given environment.
let rec evalExp (exp: Exp) (env: Env) : Val =
  match exp with
  | Num i -> Int i
  | True -> Bool true
  | False -> Bool false
  | Var x -> 
    match Map.tryFind x env with
    | Some v -> v
    | None -> raise UndefinedSemantics
  | Neg e -> 
    let v = evalExp e env
    match v with
    | Int i -> Int (-i)
    | _ -> raise UndefinedSemantics
  | Add (e1, e2) -> 
    let v1 = evalExp e1 env
    let v2 = evalExp e2 env
    match v1, v2 with
    | Int n1, Int n2 -> Int (n1 + n2)
    | _ -> raise UndefinedSemantics
  | Sub (e1, e2) -> 
    let v1 = evalExp e1 env
    let v2 = evalExp e2 env
    match v1, v2 with
    | Int n1, Int n2 -> Int (n1 - n2)
    | _ -> raise UndefinedSemantics
  | LessThan (e1, e2) ->
    let v1 = evalExp e1 env
    let v2 = evalExp e2 env
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 < n2)
    | _ -> raise UndefinedSemantics
  | GreaterThan (e1, e2) ->
    let v1 = evalExp e1 env
    let v2 = evalExp e2 env
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 > n2)
    | _ -> raise UndefinedSemantics
  | Equal (e1, e2) ->
    let v1 = evalExp e1 env
    let v2 = evalExp e2 env
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 <> n2)
    | Bool b1, Bool b2 -> Bool (b1 = b2)
    | _ -> raise UndefinedSemantics
  | NotEq (e1, e2) ->
    let v1 = evalExp e1 env
    let v2 = evalExp e2 env
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 <> n2)
    | Bool b1, Bool b2 -> Bool (b1 <> b2)
    | _ -> raise UndefinedSemantics
  | IfThenElse (e1, e2, e3) ->
    let v = evalExp e1 env
    match v with
    | Bool true -> evalExp e2 env
    | Bool false -> evalExp e3 env
    | _ -> raise UndefinedSemantics
  | LetIn (x, e1, e2) ->
    let v = evalExp e1 env
    evalExp e2 (Map.add x v env)
  | LetFunIn (f, x, e1, e2) ->
    let func = Func(x, e1, env)
    evalExp e2 (Map.add f func env)
  | LetRecIn (f, x, e1, e2) ->
    let recfunc = RecFunc(f, x, e1, env)
    evalExp e2 (Map.add f recfunc env)
  | Fun (x, e) -> Func(x, e, env)
  | App (e1, e2) ->
    let v1 = evalExp e1 env
    let v2 = evalExp e2 env
    match v1 with
    | Func(x, bodyExp, funcEnv) ->
      let newEnv = Map.add x v2 funcEnv
      evalExp bodyExp newEnv
    | RecFunc(f, x, bodyExp, funcEnv) ->
      let newEnv = Map.add x v2 (Map.add f v1 funcEnv)
      evalExp bodyExp newEnv
    | _ -> raise UndefinedSemantics
  | _ -> raise UndefinedSemantics

// Note: You may define more functions.

// The program starts execution with an empty environment. Do not fix this code.
let run (prog: Program) : Val =
  evalExp prog Map.empty
