s = "pwwkew"
s = s.lower()
largest = ""
ph = ""
for i in list(s):
  if i not in ph:
  	ph+=i
  else:
  	if len(ph) > len(largest):
  		largest = ph
  	ph = "" + i 

print(largest)
print(len(largest))
