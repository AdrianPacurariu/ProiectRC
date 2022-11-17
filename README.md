# ProiectRC
## PS2 - RC + SI (Rețele de Calculatoare + Securitatea Informației)

Program în limbajul C, o aplicație client-server în protocolul TCP, prin care se măsoară timpul care trece de la transmiterea unui mesaj de la client către server, adică timpul până la primirea răspunsului. Există posibilitatea trimiterii de mesaje de comandă serverului pentru a seta un delay înainte de transmitere, caz în care nu se realizează măsurătoarea. Întârzierea trebuie să fie >=0, altfel se va semnala eroare. Serverul este concurent.

## Exemple de utilizare
_în client_:

Mesajul:

> test

Raspuns: 
> Durata de transmitere/receptie a mesajului "test" este de 0.323 secunde.

Setare delay:

> .d 3

> test

Raspuns:
> Durata de transmitere/receptie a mesajului "test" este de 3.315 secunde.

Exemplu eronat:
> .d -1

Raspuns:
> Eroare. Delay-ul nu poate sa fie negativ.
