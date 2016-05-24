:- [dota_items].

same_elements(X, Y) :-
	msort(X, S), msort(Y, S).

/*
	Predicate `quick_buy'
	
	Take an item and add its components to the resultant list.
	Do this recursively until a component item isn't a build. Then, just add the
	item itself.
*/

quick_buy(Item, Comps) :-
	item(Item), resolve_build([Item], Comps).

resolve_build([], []).

resolve_build([Item| Tail], Out) :-
	build(Item, B), !, resolve_build(B, C), resolve_build(Tail, A),
	append(C, A, Out).

resolve_build([Item| Tail], Out) :-
	resolve_build(Tail, A), append([Item], A, Out).

/*
	Predicate `build_contains(Item, Build)'
	
	List all builds that contain Item, or vice versa.
*/

build_contains(Item, Build) :-
	item(Item), build(Build, Items), member(Item, Items).

/*
	Predicate `item_add'
	
	Take an inventory and replace an `empty` item with Item. Fail if full.
*/

/*
item_add(In, Item, Out) :-
	member(empty, In), item_add2(In, [Item], Out).

item_add2(_, [], _).

item_add2([], _, _) :-
	!, fail.

item_add2([H| T], Item, Out) :-
	item_add2(T, Item, A), .

replace_if(In, Item, Item) :-
	In == empty, !.

replace_if(In, 
*/

/*
	Predicate `resolve_inv'
	
	Take an inventory (arbitrary size) and 
*/



























