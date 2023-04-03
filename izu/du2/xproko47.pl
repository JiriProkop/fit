% Zadani c. 5:
% Napiste program resici ukol dany predikatem u5(LIN,VOUT), kde LIN je vstupni 
% celociselny seznam a VOUT je vystupni promenna, ve ktere se vraci hodnota -1/1/0,
% pokud seznam LIN obsahuje pouze licha cisla/pouze suda cisla/v ostatnich pripadech.
 
% Testovaci predikaty:                                   	% VOUT 
u5_1:- u5([5,27,-1,0, 16,-4],VOUT),write(VOUT).	        	% 0
u5_2:- u5([5,27,-1,1,15,-5],VOUT),write(VOUT).		       	% -1
u5_3:- u5([6,28,-2,0,16,-4],VOUT),write(VOUT).       		% 1
u5_r:- write('Zadej LIN: '),read(LIN),u5(LIN,VOUT),write(VOUT).

% Reseni:
u5(LIN,VOUT):-
    odd_count(LIN, Licha),
    even_count(LIN, Suda),
    (Licha =\= 0, Suda =\= 0 -> VOUT = 0 ; (Licha =:= 0 -> VOUT = 1 ; VOUT = -1)).
    
even(N):- mod(N,2) =:= 0.

odd_count([], 0).
odd_count([Head|Tail], Licha) :-
    odd_count(Tail, Licha2),
    ( \+even(Head) -> Licha is Licha2 + 1 ; Licha = Licha2 ).

even_count([], 0).
even_count([Head|Tail], Suda) :-
    even_count(Tail, Suda2),
    ( even(Head) -> Suda is Suda2 + 1 ; Suda = Suda2 ).