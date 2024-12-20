module CMinusPtr

open AST
open Types

// Evaluate expression into a value, under the given memory.
let rec evalExp (exp: Exp) (mem: Mem) : Val =
  match exp with
  | Num i -> Int i
  | True -> Bool true
  | False -> Bool false
  | LV (Var s) -> 
    match Map.tryFind s mem with
    | Some v -> v
    | None -> raise UndefinedSemantics
  | LV (Deref e) ->
    match evalExp e mem with
    | Loc varName -> 
      match Map.tryFind varName mem with
      | Some v -> v
      | None -> raise UndefinedSemantics
    | _ -> raise UndefinedSemantics
  | AddrOf varName -> 
    match Map.tryFind varName mem with
    | Some _ -> Loc varName
    | None -> raise UndefinedSemantics
  | Add (e1, e2) -> 
    let v1 = evalExp e1 mem
    let v2 = evalExp e2 mem
    match v1, v2 with
    | Int n1, Int n2 -> Int (n1 + n2)
    | _ -> raise UndefinedSemantics
  | Sub (e1, e2) -> 
    let v1 = evalExp e1 mem
    let v2 = evalExp e2 mem
    match v1, v2 with
    | Int n1, Int n2 -> Int (n1 - n2)
    | _ -> raise UndefinedSemantics
  | LessThan (e1, e2) ->
    let v1 = evalExp e1 mem
    let v2 = evalExp e2 mem
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 < n2)
    | _ -> raise UndefinedSemantics
  | GreaterThan (e1, e2) ->
    let v1 = evalExp e1 mem
    let v2 = evalExp e2 mem
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 > n2)
    | _ -> raise UndefinedSemantics
  | Equal (e1, e2) ->
    let v1 = evalExp e1 mem
    let v2 = evalExp e2 mem
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 <> n2)
    | Bool b1, Bool b2 -> Bool (b1 = b2)
    | _ -> raise UndefinedSemantics
  | NotEq (e1, e2) ->
    let v1 = evalExp e1 mem
    let v2 = evalExp e2 mem
    match v1, v2 with
    | Int n1, Int n2 -> Bool (n1 <> n2)
    | Bool b1, Bool b2 -> Bool (b1 <> b2)
    | _ -> raise UndefinedSemantics

// Execute a statement and return the updated memory.
let rec exec (stmt: Stmt) (mem: Mem) : Mem =
  match stmt with
  | NOP -> mem // NOP does not change the memory.
  | Assign (lv, e1) ->
    match lv with
    | Var s -> 
      let v = evalExp e1 mem
      Map.add s v mem
    | Deref e2 -> 
      match evalExp e2 mem with
      | Loc varName -> 
        let v = evalExp e1 mem
        Map.add varName v mem
      | _ -> raise UndefinedSemantics
  | Seq (s1, s2) ->
    let nextMem = exec s1 mem
    exec s2 nextMem
  | If (e, s1, s2) ->
    match evalExp e mem with
    | Bool true -> exec s1 mem
    | Bool false -> exec s2 mem
    | _ -> raise UndefinedSemantics
  | While (e, s) ->
    match evalExp e mem with
    | Bool true ->
      let nextMem = exec s mem
      exec stmt nextMem
    | Bool false -> mem
    | _ -> raise UndefinedSemantics

// The program starts execution with an empty memory. Do NOT fix this function.
let run (prog: Program) : Mem =
    exec prog Map.empty
