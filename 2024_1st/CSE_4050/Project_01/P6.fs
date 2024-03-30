module P6

/// From list 'l', find the element that appears most frequently in the list,
/// and return how many times it appears. If the input list is empty, return 0.
let rec helper (lst: 'a list) (cnt: Map<'a, int>) : Map<'a, int> =
  match lst with
  | [] -> cnt
  | head :: tail ->
    let newCnt =
      match Map.tryFind head cnt with
      | Some(value) -> Map.add head (value + 1) cnt
      | None -> Map.add head 1 cnt
    helper tail newCnt


let rec countMostFrequent (l: List<'a>) : int =
  let cnt = helper l Map.empty
  match Map.isEmpty cnt with
  | true -> 0
  | false ->
    let ans = Map.fold (fun max _ count -> if max > count then max else count) 0 cnt
    ans
