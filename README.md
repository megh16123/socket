# README

## Functionalities Implemented:

- Buffer Exchange  
- Retrial of messages  
- Peer Table Sharing  
- Large File(Packets) along with resend  
- Independent System Up/Down handling  

---

## How to compile the code?

To compile the code run:

```bash
make
```

---

## How to run the code?

To run the code, open at least 3 instances of the terminal. In each terminal, run:

```bash
./launcher configs/conf2
./launcher configs/config
./launcher configs/conf3
```

---

## How to test the code?


### To test the Peer Info Exchange:

Select `List Systems` option in conf2(utoob2) after the config and conf3 are up, it must display all the systems which were not present in the conf2 file.



### To test the file sharing:

1: Select `send message` option from the menu. 

2: It will ask for which system the file needs to be sent.

3: Enter the reciever of the intended file.
Enter the filename to be sent

4: If the file is successfully sent then "Message Sent" will be displayed.

5: Select `List messagges` at the reciever end to verify.