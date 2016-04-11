void DecodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData)
{
	jpeg_read_header(cinfo, TRUE);
	jpeg_start_decompress(cinfo);
	pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
	pImageData->sizeX   = cinfo->image_width;
	pImageData->sizeY   = cinfo->image_height;
	pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
	for (int i = 0; i < pImageData->sizeY; i++)
		rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height) 
	{
		rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}
	delete [] rowPtr;
	jpeg_finish_decompress(cinfo);
}

tImageJPG *LoadJPG(const char *filename)
{
	struct jpeg_decompress_struct cinfo;
	tImageJPG *pImageData = NULL;
	FILE *pFile;
	if((pFile = fopen(filename, "rb")) == NULL) 
	{
		return NULL;
	}
	jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pFile);
	pImageData = (tImageJPG*)malloc(sizeof(tImageJPG));
	DecodeJPG(&cinfo, pImageData);
	jpeg_destroy_decompress(&cinfo);
	fclose(pFile);
	return pImageData;
}
void CreateTexture(UINT textureArray[],LPSTR strFileName,int textureID,int details)
{
	if(!strFileName)									// Return from the function if no file name was passed in
		return;
	tImageJPG *pImage = LoadJPG(strFileName);			// Load the image and store the data
	if(pImage == NULL)									// If we can't load the file, quit!
		exit(0);
	glGenTextures(1, &textureArray[textureID]);
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
	if (details==0)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,2+9727);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,2+9727);	
	}
	if (details==1)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);	
	}
	if (pImage)										// If we loaded the image
	{
		if (pImage->data)							// If there is texture data
		{
			free(pImage->data);						// Free the texture data, we don't need it anymore
		}
		free(pImage);								// Free the image structure
	}
}
