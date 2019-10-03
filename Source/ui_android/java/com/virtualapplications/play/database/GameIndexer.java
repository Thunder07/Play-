package com.virtualapplications.play.database;

import android.os.Environment;

import org.apache.commons.lang3.StringUtils;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashSet;

import static com.virtualapplications.play.BootablesInterop.fullScanBootables;
import static com.virtualapplications.play.BootablesInterop.scanBootables;

public class GameIndexer
{
	private static HashSet<String> getExternalMounts()
	{
		final HashSet<String> out = new HashSet<String>();
		String reg = "(?i).*vold.*(vfat|ntfs|exfat|fat32|ext3|ext4|fuse|sdfat).*rw.*";
		StringBuilder s = new StringBuilder();
		try
		{
			final java.lang.Process process = new ProcessBuilder().command("mount")
					.redirectErrorStream(true).start();
			InputStream is = process.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));

			String line;
			while((line = reader.readLine()) != null)
			{
				if (StringUtils.containsIgnoreCase(line, "secure"))
					continue;
				if (StringUtils.containsIgnoreCase(line, "asec"))
					continue;
				if (line.matches(reg))
				{
					String[] parts = line.split(" ");
					for (String part : parts)
					{
						if (part.startsWith("/"))
							if (!StringUtils.containsIgnoreCase(part, "vold"))
								out.add(part);
					}
				}
			}
			reader.close();
			process.waitFor();
		}
		catch (final Exception e)
		{
			// SDCard Exception
		}
		return out;
	}

	private static HashSet<String> getScanDirectories()
	{
		HashSet<String> extStorage = getExternalMounts();
		extStorage.add(Environment.getExternalStorageDirectory().getAbsolutePath());
		HashSet<String> result = new HashSet<>();
		for(String anExtStorage : extStorage)
		{
			String sdCardPath = anExtStorage.replace("mnt/media_rw", "storage");
			result.add(sdCardPath);
		}
		return result;
	}

	public static void startupScan()
	{
		HashSet<String> scanDirs = getScanDirectories();
		scanBootables(scanDirs.toArray(new String[scanDirs.size()]));
	}

	public static void fullScan()
	{
		HashSet<String> scanDirs = getScanDirectories();
		if(!scanDirs.isEmpty())
		{
			fullScanBootables(scanDirs.toArray(new String[scanDirs.size()]));
		}
	}

	private boolean isNoMedia(File[] files)
	{
		for(File file : files)
		{
			if(file.getName().equalsIgnoreCase(".nomedia"))
			{
				return true;
			}
		}

		return false;
	}

	public static boolean isNoMediaFolder(File file, String storage)
	{
		return file.getAbsolutePath().contains(storage + "/Android") ||
				file.getAbsolutePath().contains(storage + "/LOST.DIR") ||
				file.getAbsolutePath().contains(storage + "/data");
	}
}
