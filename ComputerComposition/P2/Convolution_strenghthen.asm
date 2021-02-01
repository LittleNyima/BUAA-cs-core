.data
mtr1: .space 400
mtr2: .space 400
re: .space 800
space: .asciiz  " "
enter: .asciiz "\n"

.text
# $s0, $s1, $s2, $s3 for m1, n1, m2, n2
li $v0, 5
syscall
move $s0, $v0
li $v0, 5
syscall
move $s1, $v0
li $v0, 5
syscall
move $s2, $v0
li $v0, 5
syscall
move $s3, $v0

# $s4, $s5, $s6 for array pointers of mtr1, mtr2, re
la $s4, mtr1
la $s5, mtr2
la $s6, re

# $t8, $t9 for m1 - m2 + 1 and n1 - n2 + 1
sub $t8, $s0, $s2
addi $t8, $t8, 1
sub $t9, $s1, $s3
addi $t9, $t9, 1

# read martrix 1
li $t1, 0
loop1:
slt $t0, $t1, $s0
beq $t0, $0, end_loop1

li $t2, 0
inner_loop1:
slt $t0, $t2, $s1
beq $t0, $0, end_inner_loop1

li $v0, 5
syscall
sw $v0, 0($s4)
addi $s4, $s4, 4

addi $t2, $t2, 1
j inner_loop1
end_inner_loop1:

addi $t1, $t1, 1
j loop1
end_loop1:

# read martrix 2
li $t1, 0
loop2:
slt $t0, $t1, $s2
beq $t0, $0, end_loop2

li $t2, 0
inner_loop2:
slt $t0, $t2, $s3
beq $t0, $0, end_inner_loop2

li $v0, 5
syscall
sw $v0, 0($s5)
addi $s5, $s5, 4

addi $t2, $t2, 1
j inner_loop2
end_inner_loop2:

addi $t1, $t1, 1
j loop2
end_loop2:

# calculate
li $t1, 0
extern:
slt $t0, $t1, $t8
beq $t0, $0, end_extern

li $t2, 0
loop3:
slt $t0, $t2, $t9
beq $t0, $0, end_loop3

li $t3, 0
# $t5 for s, $t6 for offset
## $s5 for hi, $s6 for lo
li $s5, 0
li $s6, 0
li $t5, 0
li $t6, 0
inner_loop3:
slt $t0, $t3, $s2
beq $t0, $0, end_inner_loop3

li $t4, 0
innest_loop3:
slt $t0, $t4, $s3
beq $t0, $0, end_innest_loop3

add $t6, $t1, $t3
mul $t6, $t6, $s1
add $t6, $t6, $t2
add $t6, $t6, $t4
sll $t6, $t6, 2
lw $a1, mtr1($t6)
li $t6, 0
mul $t6, $t3, $s3
add $t6, $t6, $t4
sll $t6, $t6, 2
lw $a2, mtr2($t6)

######
#mul $a0, $a1, $a2
#add $t5, $t5, $a0
######

mthi $s5
mtlo $s6
madd $a1, $a2
mfhi $s5
mflo $s6

addi $t4, $t4, 1
j innest_loop3
end_innest_loop3:

addi $t3, $t3, 1
j inner_loop3
end_inner_loop3:

li $t6, 0
mul $t6, $t1, $t9
add $t6, $t6, $t2
sll $t6, $t6, 3
sw $s5, re($t6)
addi $t6, $t6, 4
sw $s6, re($t6)
li $t5, 0

addi $t2, $t2, 1
j loop3
end_loop3:

addi $t1, $t1, 1
j extern
end_extern:

# print
li $t1, 0
print:
slt $t0, $t1, $t8
beq $t0, $0, end_print

li $t2, 0
inner_print:
slt $t0, $t2, $t9
beq $t0, $0, end_inner_print

mul $t3, $t1, $t9
add $t3, $t3, $t2
sll $t3, $t3, 3
lw $a0, re($t3)
jal printbi
#####
#li $v0, 1
#syscall
#####
addi $t3, $t3, 4
lw $a0, re($t3)
jal printbi
#####
#li $v0, 1
#syscall
#####
li $v0, 4
la $a0, space
syscall

addi $t2, $t2, 1
j inner_print
end_inner_print:

li $v0, 4
la $a0, enter
syscall
addi $t1, $t1, 1
j print
end_print:
li $v0, 10
syscall

# $t4 to $t7 available
printbi:
move $t4, $a0
li $t5, 0
li $t6, 32
printbi_loop:
slt $t7, $t5, $t6
beq $t7, $0, end_printbi_loop

srl $a0, $t4, 31
li $v0, 1
syscall

sll $t4, $t4, 1
addi $t5, $t5, 1
j printbi_loop
end_printbi_loop:
jr $ra