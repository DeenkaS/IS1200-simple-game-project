.globl time2string   #global används så att funktionerna ska nås
                      #från andra filer
.globl delay
.globl hexasc
.globl printDing
.globl finalend
.globl randomAsm
.globl enable_interrupt
.data
.text



.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
  # CREDIT AT MATTIAS STAHRE
  hexasc:

 	PUSH $t0

 	andi  $t1, $a0, 0xF

 	slti $t0, $t1, 10 # om argumentet är mindre än 10 blir $t0 = 1. Kan också använda pesudo ble som gör båda sakerna

	beq  $t0, 1, lower	# Om $t0 är 1 hoppa till lower annars fortsätt.
	nop

 high: ## Vad är konventionen??

	 addi $v0, $t1, 55	# Addera 55 till argumentet för att göra om till ASCII-kod

	 jr $ra
	 nop

 lower:

	 addi $v0, $t1, 48	# Addera 55 till argumentet för att göra om till ASCII-kod

	 POP $t0

	 jr $ra
 	 nop


delay:					# Delay


	beq 	$a0, $zero, done	# om a0 är 0, EXIT LOOP, ## Om vi skickar in 0ms kommer programmet loopen köras en gång.
	nop


	while:
	li	$t1, 0 			# Initiera i. int i;

	for:
	slti 	$t3, $t1, 3800		# For-loopens förkrav (KONSTANTEN kan ändras)
	beq	$t3, $zero, for_end	# Hoppa om i < KONSTANTEN
	nop


	addi	$t1, $t1, 1		# i = i + 1
	j	for			# Gör att For-Loopen loopar
	nop

	for_end:
	addi	$a0, $a0, -1		# ms = ms – 1;


	bgt	$a0, $zero, while	# om ms > 0, skicka tillbaka till while.

	nop

	done:
	jr	$ra
	nop

time2string:

	# $a0 # innehåller adressen dit vi ska spara tidssträngen.
	# $a1 # tidsinformation som ska maskas 16 bit LSB


	PUSH $s1
	PUSH $s2
	PUSH $v0
	PUSH $ra
	PUSH $a0

	andi $s1, $a1, 0xffff   # Maska

	andi $t1, $a1, 0x00ff   # $s2  == 0000 0000,   Skriv DING!

	beq  $t1, $0, printDing ## Gå till printDing om 8 LSBits är noll.

# första sekundsiffran





	andi $a0, $s1, 0xf	# Kopiera maskade till argument


	jal  hexasc
	nop

	addi $t4, $v0, 0	# kopiera från v0 till t4


# andra sekundsiffran

	andi $a0, $s1, 0xf0	# Kopiera maskade till argument

	srl $a0, $a0, 4

	jal  hexasc
	nop

	addi $t5, $v0, 0	# kopiera från v0 till t4

# kolon

	addi $t6, $0, 58	# kopiera från v0 till t4

# första minutsiffran

	andi $a0, $s1, 0xf00	# Kopiera maskade till argument

	srl $a0, $a0, 8

	jal  hexasc
	nop

	addi $t7, $v0, 0	# kopiera från v0 till t4

# andra minutsiffran

	andi $a0, $s1, 0xf000	# Kopiera maskade till argument

	srl $a0, $a0, 12

	jal  hexasc
	nop

	addi $t8, $v0, 0	# kopiera från v0 till t4


####

	POP $a0



	# Första sekundsiffra
	sb $t4, 4($a0)

	# andra sekundsiffra
	sb $t5, 3($a0)

	# kolon
	sb $t6, 2($a0)

	# Första minutsiffra
	sb $t7, 1($a0)

	# Första minutsiffra
	sb $t8, 0($a0)


	# skriva in NULL

	sb $0, 5($a0)



	## Kod för Ding

	j finalend


printDing:
	 	# 474e4944
	POP $a0


	#addi $t7, $0, 0x474e	#GN
	#addi $t8, $0, 0x4944	#ID


	li $t7, 0x474e4944


	#sw $t7, -4($a0)	# GN

	sw $t7, 0($a0)		# ID



	addi $t9, $0, 0		#Nolla på slutet
	sw $t9, 4($a0)


	# sw $t9, ($a0)
finalend:


	#POP $a0
	POP $ra
	POP $v0
	POP $s2
	POP $s1




	jr $ra
	nop




  randomAsm:

  li	$v0, 41		# get time in milliseconds (as a 64-bit value)
  syscall

  move	$v0, $a0	# save the lower 32-bits of time

  jr $ra
  nop

  #END CREDIT @ MATTIAS STAHRE

  enable_interrupt:
          ei
          jr    $ra
          nop