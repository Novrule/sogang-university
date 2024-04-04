module CMinus

open AST
open Types

// Evaluate expression into a value, under the given memory.
let rec evalExp (exp: Exp) (mem: Mem) : Val =
  match exp with
  | Num i -> Int i
  | True -> Bool true
  | False -> Bool false
  | Var x -> 
    match Map.tryFind x mem with
    | Some temp -> temp
    | None -> raise UndefinedSemantics
  | Add (e1, e2) -> 
    match evalExp e1 mem, evalExp e2 mem with
    | Int n1, Int n2 -> Int (n1 + n2)
    | _ -> raise UndefinedSemantics
  | Sub (e1, e2) -> 
    match evalExp e1 mem, evalExp e2 mem with
    | Int n1, Int n2 -> Int (n1 - n2)
    | _ -> raise UndefinedSemantics
  | LessThan (e1, e2) ->
    match evalExp e1 mem, evalExp e2 mem with
    | Int n1, Int n2 -> Bool (n1 < n2)
    | _ -> raise UndefinedSemantics
  | GreaterThan (e1, e2) ->
    match evalExp e1 mem, evalExp e2 mem with
    | Int n1, Int n2 -> Bool (n1 > n2)
    | _ -> raise UndefinedSemantics
  | Equal (e1, e2) ->
    match evalExp e1 mem, evalExp e2 mem with
    | Int n1, Int n2 -> Bool (n1 = n2)
    | Bool b1, Bool b2 -> Bool (b1 = b2)
    | _ -> raise UndefinedSemantics
  | NotEq (e1, e2) ->
    match evalExp e1 mem, evalExp e2 mem with
    | Int n1, Int n2 -> Bool (n1 <> n2)
    | Bool b1, Bool b2 -> Bool (b1 <> b2)
    | _ -> raise UndefinedSemantics

// Execute a statement and return the updated memory.
let rec exec (stmt: Stmt) (mem: Mem) : Mem =
  match stmt with
  | NOP -> mem // NOP does not change the memory.
  | Assign (x, e) -> 
    let temp = evalExp e mem
    Map.add x temp mem
  | Seq (s1, s2) ->
    let nextMem = exec s1 mem
    exec s2 nextMem
  | If (condition, thenStmt, elseStmt) ->
    match evalExp condition mem with
    | Bool true -> exec thenStmt mem
    | Bool false -> exec elseStmt mem
    | _ -> raise UndefinedSemantics
  | While (condition, body) ->
    match evalExp condition mem with
    | Bool true ->
      let nextMem = exec body mem
      exec stmt nextMem
    | Bool false -> mem
    | _ -> raise UndefinedSemantics

// The program starts execution with an empty memory. Do NOT fix this function.
let run (prog: Program) : Mem =
  exec prog Map.empty
