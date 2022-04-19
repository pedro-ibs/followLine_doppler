/**
 * protocol.c
 *
 *  @date Created at:	11/03/2021 12:46:26
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * 
 *   Copyright (C) Pedro Igor B. S 2019
 * -------------------------------------------------------------------
 *
 *   Licença: GNU GPL 2
 * -------------------------------------------------------------------
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; version 2 of the
 *   License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * -------------------------------------------------------------------
 * 
 *  https://github.com/pedro-ibs/textProtocol
 * 
 * ########################################################
 *
 * Essa biblioteca foi desenvolvida para trabalhar com protocolos de
 * texto geralmente utilizados em comunicações serias como RS232
 * 
 * Com ela é possível traduzir e trabalhar com diverssos tipos de 
 * protocolos e textos padronizados como CSV
 *
 */



/* Includes ------------------------------------------------------------------*/
#include "textProtocol.h"


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const char pcTokens[] 		= TOKENS;
static const size_t cuLenToken		= sizeof(pcTokens);
/* Private Functions ---------------------------------------------------------*/


/**
 * @brief saber se há um termo dentro de outro
 * @param pcFindAt: onde procurar
 * @param pcFindThis: oque procurar
 * @return TRUE: Caso tenha a sting dento da outra
 * @return FALSE: Caso não tenha a sting dento da outra
 */
_bool textp_bFindString( CCHR *pcFindAt, CCHR *pcFindThis){
	if(strstr(pcFindAt, pcFindThis) != NULL)	return TRUE;
	return FALSE;
}


/**
 * @brief detecta se o carácter é um token
 * ver a macro TOKENS
 * @param pcTok: carácter a ser testado
 * @return TRUE: caso o carácter seja um TOKEN
 * @return FALSE: caso o carácter não seja um TOKEN
 */
_bool textp_bIsToken(CCHR pcTok){
	for(size_t uIdx=0; uIdx<cuLenToken; uIdx++){
		if(pcTokens[uIdx] == pcTok) return TRUE;
	}
	return FALSE;
}

/**
 * @brief testa se a strind é compsta apenas pro
 * numeros hexadecimais
 * @param pcStr string a ser testada;
 * @return TRUE: se a string é composta apensa por 
 * caracteres hexadecimais 
 * @return FALSE: se a string não é composta apensa
 * por caracteres hexadecimais 
 */
_bool textp_bIsHexNum(CCHR *pcStr){						// navegar pela memoria
	char cAux = 0x00;
	do{
		if(*pcStr == 0x00) break;
		cAux = *pcStr;
		if(isxdigit(cAux) == FALSE)					// testar caracteres
			return FALSE;
	}while(*pcStr++);
	return TRUE;
}


/**
 * @brief testa se a strind é compsta apenas pro
 * numeros decimais
 * @param pcStr string a ser testada;
 * @return TRUE: se a string é composta apensa por 
 * caracteres decimais 
 * @return FALSE: se a string não é composta apensa
 * por caracteres decimais 
 */
_bool textp_bIsDecNum(CCHR *pcStr){						// navegar pela memoria
	char cAux = 0x00;
	do{
		if(*pcStr == 0x00) break;
		cAux = *pcStr;
		if(isdigit(cAux) == FALSE)					// testar caracteres
			return FALSE;
	}while(*pcStr++);
	return TRUE;
}



/**
 * @brief tesa se todos os valores da string estão entre
 * 32(~) e 127( espaço ), ou seja testa se são letras da
 * tabela ascii
 * @param pcStr string a ser testada
 * @return TRUE: string stá dentro do range
 * @return FALSE: string não está dentro do range  
 */
_bool textp_bIsCaractere(CCHR *pcStr){
	for (size_t uIdx = 0; uIdx < strlen(pcStr); uIdx++){	
		if((pcStr[uIdx] > '~')){
			return FALSE;
		} else if((pcStr[uIdx] < ' ')){
			return FALSE; 
		}
	}
	return TRUE;
}

/**
 * @brief capturar informações seguida de uma label e salva em um bff:
 * 	usart = ALF=55,ALPACA=00,55,ACK=40,55,66,8468,99\n
 *					0  1  2  3
 * 	textp_bGetLabelInfo(usart, "ACK=", 3, bff);
 * 	bff=8468
 * @param pcStr: string onde será buscado a label e seu argumento 
 * @param pcLabel: label a ser buscada
 * @param cuArg: indice do argumento
 * @param pcBrr: buffer onde será copiado o argumento
 * @return TRUE: caso a informação tenha sido achada
 * @return FALSE: caso a informação não tenha sido achada
 */
_bool textp_bGetLabelInfo(CCHR *pcStr, CCHR *pcLabel, const u16 cuArg , char *pcBff){
	CCHR *pcStrAux = pcStr;
	if(textp_bFindString(pcStrAux, pcLabel) == FALSE) return FALSE;		// procurar label
	CCHR *pcAux = strstr(pcStrAux, pcLabel);
	u16 uIdxArg = 0;

	/* testar se a label está no inicio, e no caso de ser	*/
	/* uma sub string de str é virificado se há um tolken	*/
	/* isso garante que a label não seja uma sub string de	*/
	/* outra label Ex casa=5,casaco=6			*/
	if((size_t)(pcAux - pcStrAux)){
		pcAux--;
		if(textp_bIsToken(*pcAux) == FALSE) return FALSE;
		pcAux++;
	}

	/* posicionar o addr apos tolken  */
	while(textp_bIsToken(*pcAux) == FALSE){
		if(*pcLabel++ != *pcAux++) return FALSE;			// verificar igualdade da label
	}

	pcStrAux = pcAux;							// posicionar str
	pcAux = pcBff;								// salvar inicio de bff
	textp_pcCleanStr(pcBff);							// limpar bff

	/* buscar token do argumento. Exemplo:	*/
	/* temp=0,1,2,3,4,5,5,6,7;		*/
	do{
		if(textp_bIsToken(*pcStrAux++)) uIdxArg++;
	}while(uIdxArg <= cuArg);

	/* copiar argumento se arg = 4		*/
	/* no caso de temp=0,1,2,3,4,5,5,6,7;	*/
	/* o valor copiado será 4		*/
	while(textp_bIsToken(*pcStrAux) == FALSE){
		*pcBff++ = *pcStrAux++;
	}
	*pcBff=0x00;
	if(strlen(pcAux) <= 0) return FALSE;

	return TRUE;
}


/**
 * @brief lê informação de uma coluna de uma string (linha ) em formato CSV.
 * @param pcLine linha do arquivo
 * @param cuWhere posição da coluna
 * @param pcSaveTo onde será salvo a informação
 * @return TRUE: caso a informação tenha sido achada
 * @return FALSE: caso a informação não tenha sido achada
 */
_bool textp_bGetValueInCSV(CCHR *pcLine, const size_t cuWhere, char *pcSaveTo){
	for(size_t uCont=0; uCont<cuWhere; uCont++){
		/* posicionar cursor no prozimo	; se		*/
		/* nulo não há ; ou cuWhere é maior que a	*/
		/* quantidade de ;				*/
		pcLine = strchr( pcLine, ';' );
		if(pcLine == NULL) return FALSE;
		pcLine++;	// pular ; atual
	}

	/* destar se a mais um ;  */
	char *pcAux = (char*)strchr( pcLine, ';' );
	if((pcAux == NULL) && (cuWhere == 0) ){
		/* caso exita ; sem pcLine */
		return FALSE;
	}
	else if(pcAux == NULL){
		/* caso o ; selecionado	pelo cuWhere	*/
		/* seja o ultimo			*/
		strcpy(pcSaveTo, pcLine);
	}
	else{
		/* caos o ; selecionado pelo cuWhere nao seja o ultimo */
		memcpy ( pcSaveTo, pcLine, (size_t)(pcAux - pcLine));
	}

	/* se saveTo estiver vazio  */
	if(strlen(pcSaveTo) <= 0) return FALSE;

	return TRUE;
}


/**
 * @brief lé infomação de uma string dividida por alum caractere, como:
 * aaa/bbb/ccc/ddd/
 * @param pcLine linha do arquivo
 * @param cuWhere posição da coluna
 * @param pcSaveTo onde será salvo a informação
 * @return TRUE: caso a informação tenha sido achada
 * @return FALSE: caso a informação não tenha sido achada
 */
_bool textp_bGetValueInStr(CCHR *pcStr, const size_t cuWhere, CCHR div, char *pcSaveTo){
	for(size_t cont=0; cont<cuWhere; cont++){
		/* posicionar cursor no prozimo	div se	*/
		/* nulo não há div ou cuWhere é maior que	*/
		/* a quantidade de ;			*/
		pcStr = strchr( pcStr, div );
		if(pcStr == NULL) return FALSE;
		pcStr++;	// pular div atual
	}

	/* destar se a mais um   */
	char *pcAux = (char*)strchr( pcStr, div );
	if((pcAux == NULL) && (cuWhere == 0) ){
		/* caso exista div sem pcStr */
		 return FALSE;
	}
	else if(pcAux == NULL){
		/* caso o div selecionado pelo	*/
		/* Where seja o ultimo		*/
		strcpy(pcSaveTo, pcStr);
	}
	else{
		/*caos o div selecionado pelo cuWhere nao seja o ultimo */
		memcpy ( pcSaveTo, pcStr, (size_t)(pcAux - pcStr));
	}

	/* se saveTo estiver vazio  */
	if(strlen(pcSaveTo) <= 0) return FALSE;

	return TRUE;
}


/**
 * @brief escreve 0x00U em um bloco de memoria (variável)
 * @note é possível limpar outros tipos de dados alem de
 * um vetor do tipo uint8_t, basta fazer a comversão de tipo
 * de ponteiro para (u8*)
 * @param puBlk vetor da variável ha ser lipado
 * @param tamanho em byte do vetor, use sizeof
 * @return u8*: bloco de memoria/variavel "limpo"
 */
u8 *textp_puCleanBlk(u8 *puBlk, u16 uLen){
	for(size_t i=0; i<uLen; i++) puBlk[i] = 0x00U;
	return puBlk;	
}

/**
 * @brief escreve 0x00U em uma string
 * @note usar vCleanBlk para limpar a
 * string
 * @param pcStr: string ha ser limpa 
 * @return char*: string "limpa"
 */
char *textp_pcCleanStr(char *pcStr){
	textp_puCleanBlk((u8*)pcStr, strlen(pcStr));
	return pcStr;
}


/**
 * @brief completa ostring com zero a esquerda
 * @param pcStr string que será editada
 * @param cuSizeWithZeros tamanho desejado para a string  
 * @return char*: string com os zeros a esquerda inseridos
 * o tamanho real da string for 3 e o tamanho desejador for 5
 * a função completará com 0 (zero) os espaço restamante:
 * textp_pcZeroLeft( "str", 5 ) tem saida de 00str
 * 
 */
char *textp_pcZeroLeft(char *pcStr, const size_t cuSizeWithZeros){
	return textp_pcCharLeft(pcStr, cuSizeWithZeros, '0');
}

char *textp_pcCharLeft(char *pcStr, const size_t cuSizeWithZeros, char cChar){
	while (cuSizeWithZeros > strlen(pcStr)){
		size_t idx = strlen(pcStr);
		do{
			pcStr[idx+1] = pcStr[idx];
			if(idx-- == 0) break;
		}while(TRUE);
		pcStr[0] = cChar;
	}

	return pcStr;
}


/**
 * @brief Remove caracteres de uma string
 * @param pcStr, string
 * @param ccRemoveChar, caractere a ser removido
 * @return char*: string editada
 */
char *textp_pcRmChr( char *pcStr, CCHR ccRemoveChar ){				// remover um carácter de uma string de maneira porca
	unsigned char ucBff	= 0x00;						// variavel auxiliar para troca de dados
	char *pcFirstAddrs	= pcStr;
	char *pcAddr		= pcStr;					// ponteiro que apronta o endereço do ponteiro *pcStr
	do{									// loop para navegar na MEMORIA
		if(ccRemoveChar == *pcStr){					// localizar carácter ha ser removido
			pcAddr = pcStr;						// capturar endereço desse carácter
			do{
				ucBff  = *++pcStr;				// ir ao proximo endereço e  salver carácter
				*--pcStr  = ucBff;				// voltar ao endereço  sobreescrever caracer
			}while(*pcStr++);					// ir ao proximo endereço
			pcStr = pcAddr;
		}
		else pcStr++;
	}
	while( *pcStr );							// verificar final da pcStr
	return pcFirstAddrs;
}

/**
 * @brief troca todos os caracteres de uma string por outro
 * @param ccFind: carácter que deve ser procurado para troca
 * @param ccReplaceTo: caractere que irá sobreescrever ccFind
 * @return char*: string com os caracteres trocados
 */
char *textp_pcCharToChar(char *pcStr, CCHR ccFind, CCHR ccReplaceTo){
	char *pcAddr = pcStr;
	do if(*pcStr == ccFind) *pcStr = ccReplaceTo;
	while(*pcStr++);
	return pcAddr;
}


/**
 * @brief coloca todas as letras de uma string em caixa alta
 * @param pcStr que será editada
 * @return  char*: string editada
 */
char *textp_pcToUpperCase(char *pcStr){				// por pcString em caixa alta
	char *pcAddr = pcStr;
	do *pcStr = toupper(*pcStr);				// loop para navegar na MEMORIA
	while(*pcStr++);					// verificar final da str
	return pcAddr;
}

/**
 * @brief copiar uma substring
 * 
 * 
 * @return pcCopyTo
 */
char *textp_pcStrcpy(CCHR *pcStr, char *pcCopyTo,  u8 uInit, u8 uEnd){
	u8 uIdx = 0;
	if(uEnd < uInit) return pcCopyTo;
	do pcCopyTo[uIdx++] = pcStr[uInit];
	while(uEnd >= uInit++);
	pcCopyTo[--uIdx] = 0x00;
	return pcCopyTo;
}


/**
 * @brief checar se a String está dentro do valor maximo,
 * se não é NULL e se não está vazia  
 * 
 * @param pcString: string a ser checada
 * 
 * @param uMaxSize: tamanho maximo de pcString
 * 
 * @return FALSE: passou do valor maximo, ou é nula ou está vazia
 * 
 * @return TRUE: a string está no tamanho adequado
 */
_bool textp_bCheckStringSize(CCHR *pcString, const size_t uMaxSize){
	if(pcString		== NULL		) return FALSE;
	if(strlen(pcString)	>= uMaxSize	) return FALSE;
	if(strlen(pcString)	<= 0		) return FALSE;
	return TRUE;
}

/**
 * @brief testa se a string é um valor valido.
 * 
 * @note para a string ser valida ela deve ter o tamanho
 * adequado, não teve ter TOKENS como caracteres e não pode
 * conter pcIgnoreValue
 * 
 * @param pcString: string a ser checada
 * 
 * @param uMaxSize: tamanho maximo de pcString
 * 
 * @param pcIgnoreValue: caso a string tenha esse a função retrona FALSE
 */
_bool textp_bCheckStringValid(CCHR *pcString, const size_t uMaxSize, CCHR *pcIgnoreValue){
	if(textp_bCheckStringSize(pcString, uMaxSize) == FALSE){
		 return FALSE;
	}

	if(textp_bFindString(pcString, pcIgnoreValue) == TRUE){
		return FALSE;
	}

	for (size_t uIdx = 0; uIdx < strlen(pcString); uIdx++){
		if(textp_bIsToken(pcString[uIdx]) == TRUE){
			return FALSE;
		}
	}
	
	if(textp_bIsCaractere(pcString) == FALSE){
		return FALSE;
	} 

	return TRUE;

}




/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/
