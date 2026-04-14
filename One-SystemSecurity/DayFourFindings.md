# Day Four: Ransomware Techniques (mainly because I can lel)

- Documentation: [Fortinet threat research](https://www.fortinet.com/blog/threat-research/interlock-ransomware-new-techniques-same-old-tricks), [arxiv:2511.06429](https://arxiv.org/pdf/2511.06429), [Qualys analysis of the Ransomware MITRE ATT&CK FRAMEWORK top 10](https://blog.qualys.com/product-tech/2024/05/08/detect-address-the-top-10-mitre-attck-techniques-for-ransomware-using-policy-compliance)
- Date: 2026-04-14
- Status: It's working out (???) (Paper/article reads)

## Core Breakdown (more like a log than a breakdown):
1. T1486: Data Encrypted for Impact:

"Ransomware threat actors use this technique to encrypt their victims' data via cryptographic encryption algorithms. When implemented correctly, these algorithms are practically unbreakable. For improved performance, ransomware attacks use symmetric and asymmetric encryption algorithms in combination. This method is called the "hybrid encryption approach" and allows attackers to encrypt their victim's data quickly without compromising the security of their operation."

2. T1490: Inhibit System Recovery
3. T1027: Obfuscated Files or Information:
4. T1047: Windows Management Instrumentation: (Need to look into this properly)
5. T1036: Masquerading
6. T1059: Command and Scripting Interpreter: Shellcode?
7. T1562: Impair Defenses
8. T1112: Modify Registry
9. T1204: User Execution
10. T1055: Process Injection
