
  

# Homework 2

  

## Introduzione

L'homework 2 consiste nella realizzazione di un'applicazione client-server che permetta di:
- trasferire file da un client and un server (scrittura di un file)
- trasferire file da un server ad un client (lettura di un file)

L'applicazione è composta da due programmi, un client che effettua richieste di lettura e scrittura di un file, un server che riceve le richieste e gestisce le richieste dei client.

### Server
Di seguito alcuni esempi del funzionamento atteso.

Il comando:

    myFTserver -a server_address -p server_port -d ft_root_directory

esegue il programma server mettendolo in ascolto su di un determinato indirizzo IP e porta ed indicando la directory nella quale andare a scrivere/leggere i file. Se `ft_root_directory` non esiste deve essere creata.

Una volta in esecuzione, il server deve accettare connessioni da uno o piu' client e gestirle concorrentemente. Richieste di scrittura concorrenti sullo stesso file devono essere opportunamente gestite (come la richiesta di creazione concorrente di path con lo stesso nome).

Il programma server deve gestire tutte le eccezioni del caso, come ad esempio:
- richiesta di accesso a file non esistente (per la lettura)
- errore nel binding su IP e porta
- parametri di invocazione del comando errati o mancanti
- spazio su disco esaurito
- interruzione della connessione con il client

### Client
Di seguito alcuni esempi del funzionamento atteso.

Il comando:

    myFTclient -w -a server_address -p port -f local_path/filename_local -o remote_path/filename_remote

esegue il programma client, crea una connessione con il server specificato da `server_address:port`, e scrive il file `local_path/filename_local` sul server con nome `filename_remote` e nella directory specificata da `remote_path`. Il `remote_path` avrà root nella directory del server specificata con `ft_root_directory`.

Il comando:

    myFTclient -w -a server_address -p port -f local_path/filename_local

si comporta come il precedente ma il nome del path remoto e del file remoto sono gli stessi del path e file locale.

Il comando:

    myFTclient -r -a server_address -p port -f remote_path/filename_remote -o local_path/filename_local

esegue il programma client, crea una connessione con il server specificato da server_address:port e legge il file specficato da remote_path/filename_remote trasferendolo al programma client che lo scrivera' nella directory local_path assegnando il nome filename_local

Il comando:

    myFTclient -r -a server_address -p port -f remote_path/filename_remote

si comporta come il precedente ma il nome del path locale e del file locale sono gli stessi del path e file remoto.

Il comando:

    myFTclient -l -a server_address -p port -f remote_path/

permette al client di ottenere la lista dei file che si trovano in remote_path (effettua sostanzialmente un ls -la remoto). Ia lista dei file deve essere visualizzata sullo standard output del terminale da cui viene eseguito il programma myFTclient.

Il programma client deve gestire tutte le eccezioni del caso, come ad esempio:
- parametri di input errati
- file remoto non esistente (lettura)
- spazio di archiviazione insufficiente sul server (scrittura) e sul client
- interruzione della connessione con il server

## Progetto

Il progetto è strutturato nel seguente modo:
- programma server
- programma client
- parte di utility per funzionalità condivise

### Server
TODO: arricchire la documentazione.

Il file `server.h` contiene i prototipi delle funzioni ed eventuali dichiarazioni di variabili e costanti.
Il file `server.c` contiene la logica implementativa delle funzioni dichiarate nel file `server.h`.

### Client
TODO: arricchire la documentazione.

Il file `client.h` contiene i prototipi delle funzioni ed eventuali dichiarazioni di variabili e costanti.
Il file `client.c` contiene la logica implementativa delle funzioni dichiarate nel file `client.h`.  

## Startup

### Build
La fase di compilazione e linking è automatizzata mediante l'uso di Make.
Quindi vi è la presenza del file `Makefile` che definisce:
- compilazione di utils basandosi su `utils.c` e `utils.h`
- compilazione di server basandosi su `server.c` e `server.h` e creazione dello script `myFTserver`
- compilazione di client basandosi su `client.c` e `client.h` e creazione dello script `myFTclient`

Per eseguire la build del programma posizionarsi nella cartella dei sorgenti ed eseguire il comando `make` come mostrato di seguito:

    francescopio@francescopio:~/homework2$ make
    cc -c utils/utils.c
    cc -c server/server.c
    cc -o myFTserver utils.o server.o
    cc -c client/client.c
    cc -o myFTclient utils.o client.o

### Run
Per eseguire il server è necessario posizionarsi nella cartella dei sorgenti ed eseguire lo script denominato `server` nel seguente modo:

    francescopio@francescopio:~/homework2$ ./server

Per eseguire il client è necessario posizionarsi nella cartella dei sorgenti ed eseguire lo script denominato `client` nel seguente modo:

    francescopio@francescopio:~/homework2$ ./client

