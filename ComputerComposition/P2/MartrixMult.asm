.data
mtr1: .space 256
mtr2: .space 256
res: .space 256
space: .asciiz " "
enter: .asciiz "\n"

.text

# $s0 for rank of the martrix
li $v0, 5
syscall
move $s0, $v0

# $s1, $s2, $s3 for array pointers
la $s1, mtr1
la $s2, mtr2
la $s3, res

li $t1, 0
loop1:
slt $t0, $t1, $s0
beq $t0, $0, end_loop1
li $t2, 0
inner_loop1:
slt $t0, $t2, $s0
beq $t0, $0, end_inner_loop1
li $v0, 5
syscall
sw $v0, 0($s1)
addi $s1, $s1, 4
addi $t2, $t2, 1
j inner_loop1
end_inner_loop1:
addi $t1, $t1, 1
j loop1
end_loop1:
nop

li $t1, 0
loop2:
slt $t0, $t1, $s0
beq $t0, $0, end_loop2
li $t2, 0
inner_loop2:
slt $t0, $t2, $s0
beq $t0, $0, end_inner_loop2
li $v0, 5
syscall
sw $v0, 0($s2)
addi $s2, $s2, 4
addi $t2, $t2, 1
j inner_loop2
end_inner_loop2:
addi $t1, $t1, 1
j loop2
end_loop2:
nop

li $t1, 0
loop3:
slt $t0, $t1, $s0
beq $t0, $0, end_loop3
li $t2, 0
inner_loop3:
slt $t0, $t2, $s0
beq $t0, $0, end_inner_loop3
li $t3, 0
li $t4, 0 # $t4 = sum(A[i][m] * B[m][j])
innest_loop3:
slt $t0, $t3, $s0
beq $t0, $0, end_innest_loop3
# $t5, $t6 for numbers to multiply
# $t7 for offset
mul $t7, $t1, $s0
add $t7, $t7, $t3
sll $t7, $t7, 2
lw $t5, mtr1($t7)
mul $t7, $t3, $s0
add $t7, $t7, $t2
sll $t7, $t7, 2
lw $t6, mtr2($t7)

mul $t8, $t5, $t6
add $t4, $t4, $t8
addi $t3, $t3, 1
j innest_loop3
end_innest_loop3:
addi $t2, $t2, 1
sw $t4, 0($s3)
addi $s3, $s3, 4
j inner_loop3
end_inner_loop3:
addi $t1, $t1, 1
j loop3
end_loop3:
nop

li $t1, 0
la $s3, res
loop4:
slt $t0, $t1, $s0
beq $t0, $0, end_loop4
li $t2, 0
inner_loop4:
slt $t0, $t2, $s0
beq $t0, $0, end_inner_loop4
lw $a0, 0($s3)
li $v0, 1
syscall
li $v0, 4
la $a0, space
syscall
addi $s3, $s3, 4
addi $t2, $t2, 1
j inner_loop4
end_inner_loop4:
addi $t1, $t1, 1
li $v0, 4
la $a0, enter
syscall
j loop4
end_loop4:
nop

li $v0, 10
syscall