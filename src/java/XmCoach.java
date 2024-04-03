import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

public class XmCoach {
    static String mnem = "";
    static String index = "-1";
    static Map<String, String> codes = new HashMap();
    static SimpleDateFormat hudf = new SimpleDateFormat("MMM dd, yyyy");
    static SimpleDateFormat isof = new SimpleDateFormat("yyyy-MM-dd");

    public static void main(String args[]) {
        File input = new File(args[0]);
        mnem = args[1];
        Document doc = null;
        try {
            codes = loadCountryCodes("/var/www/html/country-xm.dat");
        } catch (Exception e) {
            System.err.println("Error: country codes not loaded." + e.getMessage());
        }
        try {
            doc = Jsoup.parse(input, "UTF-8", "https://transfermarkt.us");
        } catch (IOException e) {
            e.printStackTrace();
        }
        extractBio(doc);
    }

    private static void extractBio(Document doc) {
        String first = " ";
        String last = " ";
        String full = " ";
        String nat = " ";
        String isoDob = "0000-00-00";
        String pob = " ";
        String cob = " ";
        Element ehead = doc.selectFirst(".data-header");
        Element ehname = ehead.selectFirst(".data-header__headline-wrapper");
        int nc = ehname.childNodeSize();

        if (nc > 2) {
            Node nf = ehname.childNode(nc-3);
            first = nf.toString().trim();
            Node nl = ehname.childNode(nc-2);
            last = nl.childNode(0).toString().trim();
        } else {
            Node nl = ehname.childNode(nc-2);
            last = nl.childNode(0).toString().trim();
        }

        String name = String.format("%s %s", first, last).trim();
        Element eprofile = doc.selectFirst(".content-box-headline");
        Element einfo = eprofile.parent().selectFirst(".spielerdaten");
        Elements info = einfo.select("tr");
        for (Element ei : info) {
            Element eh = ei.selectFirst("th");
            Element ed = ei.selectFirst("td");
            String si = eh.childNode(0).toString().trim();
            if (si.contains("Name in Home Country") || si.contains("Full name")) {
                full = ed.childNode(0).toString().trim().replaceAll(",", "~");
            } else if (si.contains("Date of birth")) {
                if (ed.childNodeSize() > 0) {
                    Node ndob = ed.childNode(0);
                    String sda = ndob.toString().trim();
                    String sdb = sda.split("\\(")[0].trim();
                    isoDob = toIso(sdb);
                }
            } else if (si.contains("Citizenship")) {
                Node nnat = ed.selectFirst("[title]");
                if (nnat != null) {
                    String snat = nnat.attr("title");
                    nat = codes.values().contains(snat) ? snat : codes.getOrDefault(snat, " ");
                }
            } else if (si.contains("Place of Birth")) {
                Node npob = ed.selectFirst("[title]");
                if (npob != null) {
                    Node nplace = npob.previousSibling();
                    pob = nplace.toString().replaceAll("&nbsp;", "").replaceAll(",", "-").trim();
                    String scob = npob.attr("title");
                    cob = codes.values().contains(scob) ? scob : codes.getOrDefault(scob, " ");
                }
            }
        }

        System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s,%s",
            mnem, name, full, isoDob, nat, pob, cob, index));
    }

    private static String toIso(String dob) {
        if (dob==null) return "0000-00-00";

        try {
            Date d = hudf.parse(dob);
            return isof.format(d);

        } catch (ParseException e) {
            return "0000-00-00";
        }
    }


    private static Map<String, String> loadCountryCodes(String filename) throws IOException {
        Charset charset = Charset.forName("UTF-8");
        List<String> lines = Files.readAllLines(Paths.get(filename), charset);
        Map<String, String> codes = new HashMap();
        for (String s : lines) {
            String[] parts = s.split(",");
            if (parts.length > 1) {
                codes.put(parts[1], parts[0]);
            }
        }
        return codes;
    }
}
