namespace FMinus

open AST

// Type.infer() must raise this if the input program seems to have a type error.
exception TypeError

// The types available in our F- language.
type Type =
  | Int
  | Bool
  | TyVar of string
  | Func of Type * Type

type TypeEnv = Map<string, Type>

module Type =

  // Convert the given 'Type' to a string.
  let rec toString (typ: Type): string =
    match typ with
    | Int -> "int"
    | Bool -> "bool"
    | TyVar s -> s
    | Func (t1, t2) -> sprintf "(%s) -> (%s)" (toString t1) (toString t2)

  let freshTypeVar =
    let count = ref 0
    fun () ->
      let v = sprintf "t%d" !count
      count := !count + 1
      TyVar v

  let emptyEnv : TypeEnv = Map.empty

  let rec occursInType tvar typ =
    match typ with
    | TyVar v -> v = tvar
    | Func (t1, t2) -> occursInType tvar t1 || occursInType tvar t2
    | _ -> false

  let rec applySubst subst typ =
    match typ with
    | TyVar v -> 
        match Map.tryFind v subst with
        | Some t -> t
        | None -> typ
    | Func (t1, t2) -> Func (applySubst subst t1, applySubst subst t2)
    | _ -> typ

  let rec applySubstToEnv subst env =
    env |> Map.map (fun _ t -> applySubst subst t)

  let composeSubst subst1 subst2 =
    subst1 |> Map.map (fun k v -> applySubst subst2 v) |> Map.fold (fun acc k v -> Map.add k v acc) subst2

  let rec unify t1 t2 =
    match t1, t2 with
    | TyVar v, t | t, TyVar v ->
        if t = TyVar v then Map.empty
        elif occursInType v t then raise TypeError
        else Map.add v t Map.empty
    | Func (t11, t12), Func (t21, t22) ->
        let s1 = unify t11 t21
        let s2 = unify (applySubst s1 t12) (applySubst s1 t22)
        composeSubst s1 s2
    | _ when t1 = t2 -> Map.empty
    | _ -> raise TypeError

  let rec inferExp (env: TypeEnv) (exp: Exp) : Type * (Type * Type) list =
    match exp with
    | Num _ -> Int, []
    | True | False -> Bool, []
    | Var x ->
        match Map.tryFind x env with
        | Some t -> t, []
        | None -> raise TypeError
    | Neg e ->
        let t, eqs = inferExp env e
        Int, (t, Int) :: eqs
    | Add (e1, e2) | Sub (e1, e2) ->
        let t1, eqs1 = inferExp env e1
        let t2, eqs2 = inferExp env e2
        Int, (t1, Int) :: (t2, Int) :: (eqs1 @ eqs2)
    | LessThan (e1, e2) | GreaterThan (e1, e2) ->
        let t1, eqs1 = inferExp env e1
        let t2, eqs2 = inferExp env e2
        Bool, (t1, Int) :: (t2, Int) :: (eqs1 @ eqs2)
    | Equal (e1, e2) | NotEq (e1, e2) ->
        let t1, eqs1 = inferExp env e1
        let t2, eqs2 = inferExp env e2
        Bool, (t1, t2) :: (eqs1 @ eqs2)
    | IfThenElse (e1, e2, e3) ->
        let t1, eqs1 = inferExp env e1
        let t2, eqs2 = inferExp env e2
        let t3, eqs3 = inferExp env e3
        t2, (t1, Bool) :: (t2, t3) :: (eqs1 @ eqs2 @ eqs3)
    | LetIn (x, e1, e2) ->
        let t1, eqs1 = inferExp env e1
        let env' = Map.add x t1 env
        let t2, eqs2 = inferExp env' e2
        t2, eqs1 @ eqs2
    | LetFunIn (f, x, e1, e2) ->
        let tx = freshTypeVar ()
        let tr = freshTypeVar ()
        let env' = Map.add x tx (Map.add f (Func (tx, tr)) env)
        let t1, eqs1 = inferExp env' e1
        let env'' = Map.add f (Func (tx, t1)) env
        let t2, eqs2 = inferExp env'' e2
        t2, (tr, t1) :: (eqs1 @ eqs2)
    | LetRecIn (f, x, e1, e2) ->
        let tx = freshTypeVar ()
        let tr = freshTypeVar ()
        let env' = Map.add x tx (Map.add f (Func (tx, tr)) env)
        let t1, eqs1 = inferExp env' e1
        let env'' = Map.add f (Func (tx, t1)) env
        let t2, eqs2 = inferExp env'' e2
        t2, (tr, t1) :: (eqs1 @ eqs2)
    | Fun (x, e) ->
        let tx = freshTypeVar ()
        let env' = Map.add x tx env
        let tr, eqs = inferExp env' e
        Func (tx, tr), eqs
    | App (e1, e2) ->
        let t1, eqs1 = inferExp env e1
        let t2, eqs2 = inferExp env e2
        let tr = freshTypeVar ()
        tr, (t1, Func (t2, tr)) :: (eqs1 @ eqs2)
    | _ -> raise TypeError

  let solve (eqs: (Type * Type) list) : Map<string, Type> =
    let rec solveAux eqs subst =
      match eqs with
      | [] -> subst
      | (t1, t2) :: rest ->
          let subst' = unify t1 t2
          let rest' = List.map (fun (a, b) -> (applySubst subst' a, applySubst subst' b)) rest
          solveAux rest' (composeSubst subst' subst)
    solveAux eqs Map.empty
    
  // Return the inferred type of the input program.
  let infer (prog: Program) : Type =
    let t, eqs = inferExp emptyEnv prog
    let subst = solve eqs
    applySubst subst t
