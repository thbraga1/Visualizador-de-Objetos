# Visualizador de Modelos .obj

Um visualizador 3D interativo para arquivos .obj desenvolvido em C com OpenGL, baseado em GLUT. O programa permite carregar, visualizar e interagir com modelos 3D no formato Wavefront .obj.

## 🎯 Características

- **Carregamento completo de arquivos .obj**: Suporte para vértices (v), normais (vn) e faces (f)
- **Escala automática**: Modelos são automaticamente dimensionados para caber na janela
- **Centralização automática**: Modelos são centralizados na tela
- **Suporte a múltiplos formatos de face**: `v`, `v/vt`, `v//vn`, `v/vt/vn`
- **Gerenciamento dinâmico de memória**: Suporta modelos de qualquer tamanho
- **Iluminação realística**: Utiliza as normais do modelo para iluminação adequada
- **Interface interativa**: Controles de teclado e mouse para interação

## 🛠️ Pré-requisitos

### Linux/Ubuntu:
```bash
sudo apt-get install freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Fedora/CentOS:
```bash
sudo dnf install freeglut-devel mesa-libGL-devel mesa-libGLU-devel
```

### macOS:
```bash
# Xcode command line tools
xcode-select --install
# Ou via Homebrew
brew install freeglut
```

## 🚀 Compilação

```bash
gcc -o obj_viewer obj_viewer.c -lGL -lGLU -lglut -lm
```

## 💻 Como Usar

### Execução Básica
```bash
# Inicia com o modelo padrão (teapot)
./obj_viewer

# Carrega um modelo específico na inicialização
./obj_viewer modelo.obj
```

### Carregamento Interativo
1. Execute o programa
2. Pressione `L` ou `l`
3. Digite o nome do arquivo .obj
4. Pressione Enter

## 🎮 Controles

### Teclado
| Tecla | Função |
|-------|--------|
| `R` / `r` | Cor vermelha |
| `G` / `g` | Cor verde |
| `B` / `b` | Cor azul |
| `L` / `l` | Carregar arquivo .obj |
| `ESC` | Sair do programa |
| `↑` (Seta para cima) | Modo tela cheia |
| `↓` (Seta para baixo) | Janela 640x480 |

### Mouse
| Ação | Função |
|------|--------|
| Clique esquerdo | Cor aleatória |

## 📁 Estrutura do Projeto

```
obj_viewer/
├── obj_viewer.c          # Código principal
├── README.md            # Este arquivo
├── modelos/             # Pasta para modelos de teste
│   ├── teapot.obj
│   ├── cube.obj
│   └── sphere.obj
└── exemplos/            # Imagens de exemplo
```

## 🎨 Modelos de Teste

### Fontes Recomendadas:

1. **MIT CSAIL Graphics**
   - URL: `https://groups.csail.mit.edu/graphics/classes/6.837/F03/models/`
   - Modelos: teapot.obj, pumpkin.obj, teddy.obj

2. **GitHub - Common 3D Test Models**
   - URL: `https://github.com/alecjacobson/common-3d-test-models`
   - Vários modelos de teste padronizados

3. **Free3D**
   - URL: `https://free3d.com/3d-models/obj`
   - Modelos gratuitos em formato .obj

### Modelos Simples para Começar:
- **teapot.obj** - Clássico bule de teste
- **cube.obj** - Cubo básico
- **sphere.obj** - Esfera simples
- **bunny.obj** - Coelho de Stanford

## 🔧 Detalhes Técnicos

### Formato .obj Suportado
- **Vértices**: `v x y z`
- **Normais**: `vn x y z`
- **Faces**: 
  - `f v1 v2 v3` (apenas vértices)
  - `f v1/vt1 v2/vt2 v3/vt3` (vértices com texturas)
  - `f v1//vn1 v2//vn2 v3//vn3` (vértices com normais)
  - `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3` (completo)

### Características da Renderização
- **Projeção**: Ortogonal
- **Iluminação**: Phong com uma luz direcional
- **Suavização**: GL_SMOOTH para interpolação de cores
- **Normalização**: Automática para vetores normais
- **Depth Buffer**: Habilitado para renderização correta

### Algoritmo de Escala
1. Calcula bounding box do modelo (min/max X, Y, Z)
2. Determina a maior dimensão
3. Calcula fator de escala para caber em viewport de -50 a +50
4. Centraliza o modelo na origem

## 🐛 Solução de Problemas

### Erro de Compilação
```bash
# Se faltar bibliotecas OpenGL
sudo apt-get install build-essential
sudo apt-get install freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Arquivo não Encontrado
- Verifique se o arquivo .obj está no diretório correto
- Use caminho absoluto se necessário: `/home/user/modelos/teapot.obj`

### Modelo não Aparece
- Verifique se o arquivo .obj está bem formatado
- Teste com modelos simples primeiro
- Verifique se há faces definidas no arquivo

### Performance com Modelos Grandes
- O programa suporta modelos grandes, mas a performance depende do hardware
- Para modelos muito complexos (>100k faces), considere usar versões simplificadas

## 📊 Limitações

- Suporta apenas faces triangulares
- Não suporta texturas (apenas geometria e normais)
- Não suporta materiais (.mtl)
- Interface básica (sem rotação interativa)
- Renderização em modo imediato (não otimizada para modelos muito grandes)

## 🔮 Futuras Melhorias

- [ ] Rotação interativa com mouse
- [ ] Suporte a texturas
- [ ] Carregamento de materiais (.mtl)
- [ ] Múltiplos modelos simultâneos
- [ ] Exportação de screenshots
- [ ] Interface gráfica para seleção de arquivos
- [ ] Wireframe mode
- [ ] Informações estatísticas do modelo

## 📝 Licença

Este projeto é de uso educacional e acadêmico. Baseado em código fornecido para disciplinas de Computação Gráfica.



**Desenvolvido com ❤️ para aprendizado de Computação Gráfica**
