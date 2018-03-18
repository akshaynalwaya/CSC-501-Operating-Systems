	.text
	.global zfunction

zfunction:
	pushl %ebp
	movl %esp,%ebp
	
	movl 8(%ebp),%eax 
	ANDl $0xFF803FFF,%eax 
	sall $0x4,%eax 
	
	popl %ebp
	
	ret
