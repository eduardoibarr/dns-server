# DNS Server

Este projeto é um **servidor DNS básico**, implementado em C, com funcionalidades para **bloqueio**, **redirecionamento** e **cache** de domínios. Ele permite que consultas DNS sejam respondidas com base em configurações específicas, fornecendo controle sobre quais domínios estão permitidos, bloqueados ou redirecionados.

---

## Funcionalidades

1. **Resolução Recursiva de Domínios**: O servidor resolve quaisquer domínios que não estejam bloqueados ou redirecionados, consultando servidores DNS externos.
2. **Redirecionamento de Domínios**: Responde a consultas de domínios configurados no arquivo \`redirects.txt\`, redirecionando-os para endereços IP específicos.
3. **Bloqueio de Domínios**: Ignora consultas de domínios presentes no arquivo \`blocked_domains.txt\`, impedindo o acesso a esses domínios.
4. **Cache de Consultas**: Otimiza a resposta para consultas frequentes, armazenando resultados em cache e melhorando a performance.
5. **Compatível com o Protocolo UDP**: O servidor usa o protocolo UDP na porta especificada, respondendo diretamente a consultas DNS do cliente.

---

## Requisitos

- **Compilador C**: GCC ou equivalente.
- **Ferramentas de Rede**: \`dig\` ou similar para testes de consulta DNS.

---

## Configuração

### 1. Crie os arquivos de dados

Para configurar domínios específicos para bloqueio e redirecionamento, você precisa preencher os arquivos \`blocked_domains.txt\` e \`redirects.txt\` dentro do diretório \`data/\`:

#### \`blocked_domains.txt\`

Cada linha representa um domínio que será bloqueado pelo servidor. Exemplo:

``
badexample.com
malware.com
``

#### \`redirects.txt\`

Cada linha contém um domínio seguido pelo IP para redirecionamento, separados por uma vírgula. Exemplo:

``
example.com,127.0.0.1
test.com,192.168.1.1
``

### 2. Compilação

Para compilar o projeto, execute os seguintes comandos no diretório do projeto:

``
make clean
make all
``

Esses comandos irão compilar tanto o servidor DNS (\`dns_server\`) quanto os executáveis de teste, armazenando os arquivos em \`build/\`.

---

## Executando o Servidor

### Iniciar o Servidor DNS

Para iniciar o servidor DNS, execute:

``
./dns_server
``

A saída indicará que o servidor está rodando e escutando na porta padrão DNS (53).

> **Nota**: Se você encontrar problemas de permissão na porta 53, altere o valor da constante \`PORT\` no arquivo \`dns_server.h\` para uma porta não privilegiada, como 1053, e recompile o projeto.

### Consultando o Servidor DNS

Para testar o servidor DNS, use o comando \`dig\` ou um cliente similar. Por exemplo, para consultar um domínio que você configurou no \`redirects.txt\`:

``
dig @127.0.0.1 -p 53 example.com
``

Se o domínio \`example.com\` estiver configurado no \`redirects.txt\`, o servidor deverá retornar o IP configurado, como \`127.0.0.1\`.

---

## Executando os Testes

O projeto inclui uma série de testes para verificar as funcionalidades de bloqueio, redirecionamento e resolução. Para executar todos os testes, use:

``
make test
``

Os testes irão compilar e executar arquivos de teste específicos e verificar se os módulos do servidor estão funcionando corretamente. Os resultados dos testes serão exibidos no terminal.

---

## Limpeza

Para remover arquivos compilados, incluindo o servidor e os executáveis de teste, use:

``
make clean
``

Este comando remove os arquivos \`.o\` e os executáveis do diretório \`build/\`, deixando o diretório pronto para uma nova compilação limpa.

---

## Notas Importantes

- **Segurança**: O servidor DNS responde a todas as consultas, exceto aquelas para domínios bloqueados em \`blocked_domains.txt\`.
- **Resolução Recursiva**: O servidor realiza resolução recursiva para domínios não presentes nas listas de bloqueio ou redirecionamento, consultando servidores DNS externos.
- **Permissões de Porta**: Recomenda-se alterar a porta padrão para um valor acima de 1024 (por exemplo, 1053) no arquivo \`dns_server.h\` para evitar problemas de permissão.

---

## Possíveis Expansões

- **Implementação de TTL Dinâmico**: Configurar diferentes valores de TTL para entradas de cache.
- **Suporte a IPv6**: Adicionar suporte para resolver endereços IPv6.
- **Logs Avançados**: Melhorar o sistema de logs para monitorar o tráfego e detectar possíveis ameaças.
- **Interface de Gerenciamento**: Desenvolver uma interface para facilitar a gestão das listas de bloqueio e redirecionamento.
- **Segurança Avançada**: Implementar verificações de segurança adicionais, como proteção contra ataques de amplificação DNS.

---

Este projeto foi desenvolvido com foco em oferecer um controle básico sobre o fluxo de consultas DNS em uma rede, permitindo que domínios específicos sejam bloqueados ou redirecionados. Para contribuições e sugestões, sinta-se à vontade para abrir uma _issue_ ou _pull request_.

---