## Nejednoznacnosti zadani s mym reseni

zatim nic

## Zajimava reseni

Portnum 0 je default a neda se zadat jako argumen z duvodu predani chyby od strtol

## Problemky

Pokud prijde jenom 1 klient, proces, ktery se mu prideli zustane, jako zombie, dokud se neobjevi dalsi klient.
Je to kvuli tomu, ze funkce recvfrom, diky ktere server ceka na danem portu na zpravu, je blokujici, takze
kvuli ni neni mozne pravidelne kontrolovat zombie procesy.

Kdyz se serveru posle ctrl c / SIGINT, tak se ukonci a parentem vsech child procesu se stane systemovy init(PID 0),
ktery je potom kontroluje, takze nikdy nezustanou zombie nadlouho. To ale taky znamena, ze servery dokonci vsechny probihajici
interakce a az potom uplne skonci.

## timeout

timeout je nastaven u recvfrom protoze je to tak nejjednodusi a resend se dela podle hodnoty v codes.h .
Kdyz mi teda posle packet nekdo cizi - client jakoby prijde o pokus.
